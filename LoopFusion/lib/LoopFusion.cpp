#include "LoopFusion.hpp"

#include <cstddef>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CFG.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>
#include <vector>

using namespace llvm;

/*
 * implementazione custom per determinare IV di un loop
 * basata su:
 * https://github.com/llvm/llvm-project/blob/95d13c01ecba5c9dba8ea1bd875c4179cbaea9e2/llvm/lib/Transforms/Scalar/LoopInterchange.cpp#L298
 * per info:
 * https://discourse.llvm.org/t/obtaining-bounds-of-loops/57689/2
 */
PHINode* LoopFusion::getInductionVariable(Loop* L, ScalarEvolution& SE) const
{
    // se esiste una IV canonica, la ritorno
    PHINode* canonicalIV = L->getCanonicalInductionVariable();
    if (canonicalIV)
        return canonicalIV;
    // esploro le phi del blocco header per andare alla ricerca dell'IV
    // L'IV i questione deve essere della forma A + B*x con A e B valori loop invariant (isAffine());
    // l'operatore deve essere di somma (SCEVAddRecExpr);
    // lo step deve essere constante (SCEVConstant)
    for (auto& phi : L->getHeader()->phis())
    {
        // controllo espressione scev
        SCEVAddRecExpr const* addrc = dyn_cast<SCEVAddRecExpr>(SE.getSCEV(&phi));
        if (!addrc || !addrc->isAffine())
            continue;
        // controllo step scev
        SCEV const* step = addrc->getStepRecurrence(SE);
        if (isa<SCEVConstant>(step))
            return &phi;
    }
    // se tutto fallisce, ritorno null
    return nullptr;
}

bool LoopFusion::areLoopsAdjacent(const Loop* i, const Loop* j) const
{
    const BasicBlock* iExitBlock = i->getExitBlock();
    const BasicBlock* jPreheader = j->getLoopPreheader();

    return iExitBlock != nullptr &&
           jPreheader != nullptr &&
           iExitBlock == jPreheader &&
           jPreheader->getInstList().size() == 1;
    /*
     * Non c'è bisogno di controllare anche il tipo dell'istruzione contenuta nel preheader.
     * Per definizione, il preheader di un loop contiene un'istruzione branch all'header del loop.
     * Se quindi nel preheader c'è una sola istruzione, sicuramente quella è una branch.
     */
}

void LoopFusion::findAdjacentLoops(const std::vector<Loop*>& loops, std::vector<std::pair<Loop*, Loop*>>& adjLoopPairs) const
{
    auto allLoopsSubLoopsRange = make_filter_range(
        map_range(loops, [](const Loop* loop) -> const std::vector<Loop*>& {
            return loop->getSubLoops();
        }),
        [](const std::vector<Loop*>& subLoops) {
            // per evitare chiamate ricorsive inutili, considero solo i loop che hanno almeno 2 loop interni
            return subLoops.size() > 1;
        });

    for (const auto& subLoops : allLoopsSubLoopsRange)
    {
        findAdjacentLoops(subLoops, adjLoopPairs);
    }

    // itero in [1, loop.size()) anziché [0, loops.size() - 1) per evitare negative overflow quando loops.size() == 0
    for (size_t i = 1; i < loops.size(); i++)
    {
        Loop* loopi = loops[i - 1];
        Loop* loopj = loops[i];

        if (areLoopsAdjacent(loopi, loopj))
        {
            outs() << "Ho trovato due loop adiacenti:\n";
            outs() << " - ";
            loopi->print(outs(), false, false);
            outs() << "\n - ";
            loopj->print(outs(), false, false);
            outs() << "\n";
            adjLoopPairs.emplace_back(loopi, loopj);
        }
    }
}

bool LoopFusion::haveSameTripCount(ScalarEvolution& SE, Loop const* loopi, Loop const* loopj) const
{
    unsigned int tripi = SE.getSmallConstantTripCount(loopi);
    unsigned int tripj = SE.getSmallConstantTripCount(loopj);
    return tripi != 0 && tripj != 0 && tripi == tripj;
}

bool LoopFusion::areControlFlowEquivalent(DominatorTree& DT, PostDominatorTree& PT, const Loop* loopi, const Loop* loopj) const
{
    return DT.dominates(loopi->getHeader(), loopj->getHeader()) &&
           PT.dominates(loopj->getHeader(), loopi->getHeader());
}

bool LoopFusion::checkNegativeDistanceDeps(Loop* loopi, Loop* loopj) const { return true; }

bool LoopFusion::mergeLoops(Loop* loopFused, Loop* loopToFuse, ScalarEvolution& SE, LoopInfo& LI) const
{
    PHINode* loopToFuseIndV = getInductionVariable(loopToFuse, SE);
    PHINode* loopFusedIndV = getInductionVariable(loopFused, SE);

    if (!loopToFuseIndV || !loopFusedIndV)
        return false;

    // sostituisco usi della variabile induttiva nel secondo loop
    loopToFuseIndV->replaceAllUsesWith(loopFusedIndV);

    // header loop principale dovrà puntare all'uscita del loop secondario
    loopFused->getHeader()->getTerminator()->replaceSuccessorWith(loopFused->getExitBlock(), loopToFuse->getExitBlock());

    // mettere body secondo loop in primo loop
    // tutti i predecessori del latch del loop primario devono puntare al blocco di entry dopo l'header del loop secondario
    // è importante effettuare prima questa manipolazione del cfg, in quanto i predecessori del latch del loop primario andranno a cambiare
    // in seguito
    BasicBlock* entryToSecondaryBody = nullptr;
    BasicBlock* secondaryHeader = loopToFuse->getHeader();
    BasicBlock* secondaryLatch = loopToFuse->getLoopLatch();
    BasicBlock* primaryLatch = loopFused->getLoopLatch();

    for (BasicBlock* succ : successors(secondaryHeader))
    {
        if (loopToFuse->contains(succ))
        {
            entryToSecondaryBody = succ;
            break;
        }
    }

    for (BasicBlock* pred : predecessors(primaryLatch))
    {
        pred->getTerminator()->replaceSuccessorWith(primaryLatch, entryToSecondaryBody);
    }

    // tutti i predecessori del latch del loop secondario devono puntare al latch del loop primario

    for (BasicBlock* pred : predecessors(secondaryLatch))
    {
        pred->getTerminator()->replaceSuccessorWith(secondaryLatch, primaryLatch);
    }

    // faccio puntare l'header del secondo loop al suo latch, lasciandolo vuoto
    loopToFuse->getHeader()->getTerminator()->replaceSuccessorWith(entryToSecondaryBody, secondaryLatch);

    // posso aggiornare anche l'analisi sui loop
    // considero solo lo spostamento dei blocchi da un loop all'altro e la cancellazione del secondo loop
    // bisognerebbe considerare anche i loop innestati: rimuoverli dal secondo e aggiungerli al primo
    // effettuando una copia dei blocchi del loop posso iterare e modificare il loop stesso
    for (auto& bb : loopToFuse->blocks())
    {
        // header e latch rimarranno soli, non devono essere spostati!
        if (bb != secondaryHeader && bb != secondaryLatch)
        {
            loopFused->addBasicBlockToLoop(bb, LI);
            loopToFuse->removeBlockFromLoop(bb);
        }
    }

    // finchè il loop che è stato fuso non diventa Innermost (ossia non contiene altri loop)
    // continuo a "trasferire" loop figli verso l'altro loop
    while (!loopToFuse->isInnermost())
    {
        // prendo solo il primo figlio e continuo fin quando non ho finito
        Loop* child = *(loopToFuse->begin());
        loopToFuse->removeChildLoop(child);
        loopFused->addChildLoop(child);
    }

    LI.erase(loopToFuse);

    return true;
}

PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& fam)
{
    PostDominatorTree& PT = fam.getResult<PostDominatorTreeAnalysis>(function);
    DominatorTree& DT = fam.getResult<DominatorTreeAnalysis>(function);
    ScalarEvolution& SE = fam.getResult<ScalarEvolutionAnalysis>(function);
    LoopInfo& loopInfo = fam.getResult<LoopAnalysis>(function);
    const std::vector<Loop*>& topLevelLoops = loopInfo.getTopLevelLoops();
    const std::vector<Loop*>& topLevelLoopsInPreorder = std::vector(topLevelLoops.rbegin(), topLevelLoops.rend());
    std::vector<std::pair<Loop*, Loop*>> adjacentLoops;

    findAdjacentLoops(topLevelLoopsInPreorder, adjacentLoops);

    const auto& loopsToMerge = make_filter_range(adjacentLoops, [this, &PT, &DT, &SE](std::pair<Loop*, Loop*> pair) {
        return haveSameTripCount(SE, pair.first, pair.second) &&
               areControlFlowEquivalent(DT, PT, pair.first, pair.second) &&
               checkNegativeDistanceDeps(pair.first, pair.second);
    });

    std::vector<std::pair<Loop*, Loop*>> loopsToMergeVector(loopsToMerge.begin(), loopsToMerge.end());

    for (Loop* l : topLevelLoops)
        outs() << *l << '\n';

    for (int i = 0; i < loopsToMergeVector.size(); i++)
    {
        bool merged = mergeLoops(loopsToMergeVector[i].first, loopsToMergeVector[i].second, SE, loopInfo);

        /*
         * gestisco caso di "adiacenza transitiva": se A è adiacente a B e B è adiacente a C, allora A è adiacente a C;
         * per come è stato esplorato l'albero dei loop, avrò le seguenti coppie: (A, B), (B, C)
         * se B viene fuso in A, allora mi basta sostituire nella seconda coppia il loop B con il loop (modificato) A.
         */
        if (merged && i != loopsToMergeVector.size() - 1 && loopsToMergeVector[i].second == loopsToMergeVector[i + 1].first)
        {
            loopsToMergeVector[i + 1].first = loopsToMergeVector[i].first;
        }
    }

    for (Loop* l : loopInfo.getTopLevelLoops())
        outs() << *l << '\n';

    return PreservedAnalyses::none();
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "CustomLoopFusion",
        .PluginVersion = LLVM_VERSION_STRING,
        .RegisterPassBuilderCallbacks =
            [](PassBuilder& PB) {
                PB.registerPipelineParsingCallback(
                    // CREO LO STUB per il mio TestPass
                    //
                    // RICORDA: Posso usare (coi relativi Pass Managers)
                    // -------------------------------------------------
                    // ModulePass
                    // CallGraphSCCPass
                    // FunctionPass
                    // LoopPass
                    // RegionPass
                    // BasicBlockPass
                    [](StringRef name, FunctionPassManager& passManager, ArrayRef<PassBuilder::PipelineElement>) -> bool {
                        if (name == "custom-loopfusion")
                        {
                            passManager.addPass(LoopFusion());
                            return true;
                        }

                        return false;
                    });
            }};
}
