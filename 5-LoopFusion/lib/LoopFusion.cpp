#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/IR/Dominators.h"
#include <llvm/IR/Function.h>
#include "llvm/Analysis/ScalarEvolution.h"
#include <llvm/IR/PassManager.h>

using namespace llvm;

namespace
{
  bool areNormalizedLoop(Loop *L1, Loop *L2);
  bool areLoopsAdjacent(Loop *L1, Loop *L2, LoopInfo &LI);
  bool haveLoopsSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE);
  bool haveControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT);
  void loopFusion(Loop *L1, Loop *L2);
  void switchInductionVariable(Loop *L1, Loop *L2);
  void modifyCFG(Loop *L1, Loop *L2);

  class LoopFusion final : public FunctionPass
  {
  public:
    static char ID;

    LoopFusion() : FunctionPass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<ScalarEvolutionWrapperPass>();
      AU.addRequired<DominatorTreeWrapperPass>();
    }

    virtual bool runOnFunction(Function &F) override
    {
      outs() << "\nLOOPFUSIONPASS INIZIATO...\n\n";

      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
      DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();

      Loop *nextLoop = nullptr;
      for (Loop *L : LI)
      {
        if (nextLoop)
        {
          outs() << "LOOP predecessor: " << *nextLoop;
          outs() << "LOOP: " << *L << "\n";
          outs() << "\tAnalyzing...\n";

          if (!areLoopsAdjacent(L, nextLoop, LI))
          {
            outs() << "\tLOOPS are not adjacent\n";
            continue;
          }
          if (!areNormalizedLoop(L, nextLoop))
          {
            outs() << "\tLOOPS are not normalized\n";
            continue;
          }
          if (!haveLoopsSameTripCount(L, nextLoop, SE))
          {
            outs() << "\tLOOPS have not same trip count\n";
            continue;
          }
          if (!haveControlFlowEquivalent(L, nextLoop, DT))
          {
            outs() << "\tLOOPS have not control flow equivalent\n";
            continue;
          }

          outs() << "\tLOOPS are fusionable\n";

          loopFusion(L, nextLoop);
        }
        nextLoop = L;
      }

      return true;
    }
  };

  char LoopFusion::ID = 0;
  RegisterPass<LoopFusion> X("loop-fusion-pass", "Loop Fusion Pass");

  void loopFusion(Loop *L1, Loop *L2)
  {
    switchInductionVariable(L1, L2);
    modifyCFG(L1, L2);
  }
  void switchInductionVariable(Loop *L1, Loop *L2)
  {
    PHINode *inductionVariableL1 = L1->getCanonicalInductionVariable();
    PHINode *inductionVariableL2 = L2->getCanonicalInductionVariable();

    for (BasicBlock *BB : L2->getBlocks())
    {
      for (Instruction &I : *BB)
      {
        for (Use &U : I.operands())
        {
          if (U.get() == inductionVariableL2)
          {
            U.set(inductionVariableL1);
          }
        }
      }
    }
  }
  void modifyCFG(Loop *L1, Loop *L2)
  {
    // NON FUNZIONA
    BasicBlock *ExitBlockL1 = L1->getExitBlock();
    BasicBlock *HeaderL2 = L2->getHeader();

    BranchInst *BranchToHeaderL2 = BranchInst::Create(HeaderL2, ExitBlockL1);
    ExitBlockL1->getTerminator()->eraseFromParent();
    ExitBlockL1->getInstList().push_back(BranchToHeaderL2);
  }

  bool areNormalizedLoop(Loop *L1, Loop *L2)
  {
    return L1->isLoopSimplifyForm() && L2->isLoopSimplifyForm();
  }
  bool areLoopsAdjacent(Loop *L1, Loop *L2, LoopInfo &LI)
  {
    return L1->getExitBlock() == L2->getLoopPreheader();
  }
  bool haveLoopsSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE)
  {
    unsigned int tripCountValueL1 = SE.getSmallConstantTripCount(L1);
    unsigned int tripCountValueL2 = SE.getSmallConstantTripCount(L2);
    outs() << "\t\tLoop 1 trip count: " << tripCountValueL1 << "\n";
    outs() << "\t\tLoop 2 trip count: " << tripCountValueL2 << "\n";

    return tripCountValueL1 == tripCountValueL2;
  }
  bool haveControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT)
  {
    return DT.dominates(L1->getExitBlock(), L2->getLoopPreheader()) && DT.dominates(L2->getLoopPreheader(), L1->getExitBlock());
  }
} // namespace
