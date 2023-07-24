#include "LocalOpts.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

void TransformPass::getAnalysisUsage(AnalysisUsage &AU){
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<LoopInfoWrapperPass>();
}

bool TransformPass::isPreHeaderExitBlock(BasicBlock* block1, BasicBlock* block2) {
  if(block1 != nullptr && block2 != nullptr && block1 == block2 && (block2->size() == 1)) 
  {
    assert((block2->begin()->getOpcode()) == Instruction::Br);
    return true;
  }
  return false;
}

bool TransformPass::equalTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE)
{
  if(SE.getSmallConstantTripCount(L1) != 0 && SE.getSmallConstantTripCount(L2) != 0 && SE.getSmallConstantTripCount(L1) == SE.getSmallConstantTripCount(L2))
    return true;
  return false;
}

bool TransformPass::controlFlowEq(BasicBlock* block1, BasicBlock* block2,  DominatorTree &DT, PostDominatorTree &PDT )
{
  if(DT.dominates(block1, block2) && PDT.dominates(block2, block1)) //
    return true;
  return false;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &AM) 
{
  //PER TESTARE: opt -load-pass-plugin=./libLocalOpts.so -passes=transform test/Loop.opt.ll -S -o test/test.optimized.ll
  bool change = true;
  while(change)
  {
    change = false;
    auto &LI = AM.getResult<LoopAnalysis>(F); //per ottenere gli header del loop
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F); //per capire se i loop hanno lo stesso numero di iterazioni
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F); 
    SmallVector< Loop *, 2 > getLoops = LI.getLoopsInPreorder(); //lista di loop dall'alto verso il basso

    for(int i = 0; i < getLoops.size() - 1; i++)
    {
      auto *L1 = getLoops[i];
      auto *L2 = getLoops[i+1];
      if( isPreHeaderExitBlock(L1->getExitBlock(),L2->getLoopPreheader()) && equalTripCount(L1, L2, SE) && controlFlowEq(L1->getLoopPreheader(),L2->getLoopPreheader(),DT,PDT) )
      {
        outs()<<"\nsono stati trovati 2 cicli dove applicare la loop fusion\n\n";
        //prendo le 2 induction variables dei 2 loop
        PHINode *InductionVar1 = L1->getCanonicalInductionVariable();
        PHINode *InductionVar2 = L2->getCanonicalInductionVariable();

        //sostituisco la seconda induction variable con la prima
        InductionVar2->replaceAllUsesWith(InductionVar1);

        // l'uscita del primo blocco diventa l'uscita del secondo blocco
        L1->getHeader()->getTerminator()->replaceSuccessorWith(L1->getExitBlock(), L2->getExitBlock());
        
        //recupero il blocco di entrata nel body del secondo loop
        BasicBlock* L2BodyEntry = nullptr;
        BasicBlock* L2Header = L2->getHeader();

        for (BasicBlock* succ : successors(L2Header))
        {
            if (L2->contains(succ))
            {
                L2BodyEntry = succ;
                break;
            }
        }

        //collego il body del primo loop col body del secondo loop
        BasicBlock* primaryLatch = L1->getLoopLatch();

        //prendo il predecessore del latch del primo loop ovvero l'ultimo blocco del body del primo loop
        for (BasicBlock* pred : predecessors(primaryLatch))
        {
          pred->getTerminator()->replaceSuccessorWith(primaryLatch, L2BodyEntry);
        }

        //sostituisco il latch del secondo loop al latch del primo loop (cambio i puntatori)
        BasicBlock* secondaryLatch = L2->getLoopLatch();

        for (BasicBlock* pred : predecessors(secondaryLatch))
        {
            pred->getTerminator()->replaceSuccessorWith(secondaryLatch, primaryLatch);
        }

        //collego l'entrata del body del secondo loop con il suo latch per non farlo eseguire mai più,
        //così verrà rimosso nei successivi passi di ottimizzazione di llvm
        L2->getHeader()->getTerminator()->replaceSuccessorWith(L2BodyEntry, secondaryLatch);

        //aggiusto i collegamenti del secondo blocco senza toccare header e latch del secondo loop
        for (auto& bb : L2->blocks())
        {
            // header e latch non devono essere spostati
            if (bb != L2Header && bb != secondaryLatch)
            {
                L1->addBasicBlockToLoop(bb, LI);
                L2->removeBlockFromLoop(bb);
            }
        }

        //aggiusto i loop innestati se ce ne sono
        while (!L2->isInnermost())
        {
            // prendo solo il primo figlio e continuo fin quando non ho finito
            Loop* child = *(L2->begin());
            L2->removeChildLoop(child);
            L1->addChildLoop(child);
        }

        // Rimuovere il loop 2
        LI.erase(L2);

        outs()<<"fase di merge completata con successo\n\n";
      }

      AM.invalidate(F, PreservedAnalyses::none());
      //F.dump();
      change = true;
      break;
    }

  }
  
  return PreservedAnalyses::none();
}

char TransformPass::ID = 0;
