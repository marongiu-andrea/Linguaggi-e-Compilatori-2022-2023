#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

using namespace llvm;

bool areBlocksAdjacent(const llvm::BasicBlock* block1, const llvm::BasicBlock* block2) {

    const llvm::Instruction* terminator = block1->getTerminator();
 
        // Verifica se il ramo non condizionale punta al secondo basic block
        if (terminator->getSuccessor(0) == block2) {
            return true;
        }

    return false;
}

bool isLoopControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT) {
  // Check if L1 dominates L2
  if (!DT.dominates(L1->getHeader(), L2->getHeader()))
    return false;

  // Check if L2 post-dominates L1
  if (!PDT.dominates(L2->getHeader(), L1->getHeader()))
    return false;

  return true;
}

PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &AM) {

  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  int l = 0;
  int BID = 0;
  
  SmallVector<Loop*> PreOrderLoops = LI.getLoopsInPreorder();
  Loop *LP = nullptr;
  for(Loop *L: PreOrderLoops){
    outs() << "PROCESSING LOOP> " << ++l << "\n";
    if(LP == nullptr)
    {
      LP = L;
      continue;
    }
    if (areBlocksAdjacent(LP->getExitBlock(),L->getHeader()))
    {
      outs() << LP << " è un loop adiacente a " << L << "\n";
      const SCEV *TripCountSCEVLP = SE.getBackedgeTakenCount(LP);
      const APInt TripCountLP = dyn_cast<SCEVConstant>(TripCountSCEVLP)->getAPInt();
      const SCEV *TripCountSCEVL = SE.getBackedgeTakenCount(L);
      const APInt TripCountL = dyn_cast<SCEVConstant>(TripCountSCEVL)->getAPInt();
      if(TripCountLP == TripCountL)
      {
        outs() << "I due Loop hanno lo stenno numero di iterazioni: "<< TripCountL << "\n";
      }
      else{
        continue;
      }

      if(isLoopControlFlowEquivalent(LP,L,DT,PDT))
      {
        outs() << "I due loop sono Control Flow Equivalent " << "\n";
      }
    }
    else{
      continue;
    }
  }

  return PreservedAnalyses::none();
}

