#include "LoopFusion.h"

#include <cstddef>
#include <llvm/ADT/STLExtras.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>
#include <vector>

using namespace llvm;

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

void LoopFusion::findAdjacentLoops(const std::vector<Loop*>& loops) const
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
        findAdjacentLoops(subLoops);
    }

    // itero in [1, loop.size()) anziché [0, loops.size() - 1) per evitare negative overflow quando loops.size() == 0
    for (size_t i = 1; i < loops.size(); i++)
    {
        const Loop* loopi = loops[i - 1];
        const Loop* loopj = loops[i];

        if (areLoopsAdjacent(loopi, loopj))
        {
            outs() << "Ho trovato due loop adiacenti:\n";
            outs() << " - ";
            loopi->print(outs(), false, false);
            outs() << "\n - ";
            loopj->print(outs(), false, false);
            outs() << "\n";
        }
    }
}

PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& fam)
{
    const LoopInfo& loopInfo = fam.getResult<LoopAnalysis>(function);
    const std::vector<Loop*>& topLevelLoops = loopInfo.getTopLevelLoops();
    const std::vector<Loop*>& topLevelLoopsInPreorder = std::vector(topLevelLoops.rbegin(), topLevelLoops.rend());

    findAdjacentLoops(topLevelLoopsInPreorder);

    return PreservedAnalyses::all();
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
