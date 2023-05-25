#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/IR/Dominators.h"
#include <llvm/Analysis/PostDominators.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace
{
  bool areNormalizedLoop(Loop *L1, Loop *L2);
  bool areLoopsAdjacent(Loop *L1, Loop *L2, LoopInfo &LI);
  bool haveLoopsSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE);
  bool haveCanonicalInductionVariable(Loop *L1, Loop *L2);
  bool haveControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT);
  void switchInductionVariables(Loop *L1, Loop *L2);
  SmallVector<BasicBlock *> getBodyBlocks(Loop *L);
  void loopFusion(Loop *L1, Loop *L2, DominatorTree &DT, LoopInfo &LI);

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
      AU.addRequired<PostDominatorTreeWrapperPass>();
    }

    virtual bool runOnFunction(Function &F) override
    {
      outs() << "\nLOOPFUSIONPASS INIZIATO...\n\n";

      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
      DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
      PostDominatorTree &PDT = getAnalysis<PostDominatorTreeWrapperPass>().getPostDomTree();

      Loop *nextLoop = nullptr;
      for (Loop *L : LI)
      {
        if (nextLoop)
        {
          outs() << "LOOP predecessor: " << *nextLoop;
          outs() << "LOOP: " << *L << "\n";
          outs() << "\tAnalyzing...\n\n";

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
          if (!haveCanonicalInductionVariable(L, nextLoop))
          {
            outs() << "\tLOOPS have not canonical induction variable\n";
            continue;
          }
          if (!haveControlFlowEquivalent(L, nextLoop, DT, PDT))
          {
            outs() << "\tLOOPS have not control flow equivalent\n";
            continue;
          }

          loopFusion(L, nextLoop, DT, LI);
          outs() << "\tLOOPS fused\n";
        }
        nextLoop = L;
      }

      return true;
    }
  };

  char LoopFusion::ID = 0;
  RegisterPass<LoopFusion> X("loop-fusion-pass", "Loop Fusion Pass");

  void loopFusion(Loop *L1, Loop *L2, DominatorTree &DT, LoopInfo &LI)
  {
    switchInductionVariables(L1, L2);

    SmallVector<BasicBlock *> bodyL2 = getBodyBlocks(L2);

    // printBlocks(bodyL2);

    BasicBlock *lastBasicBlockBodyL1 = getBodyBlocks(L1).back();

    Instruction *terminatorBodyL1 = lastBasicBlockBodyL1->getTerminator();

    // Move the body of L2 next to the body of L1
    for (auto &bb : bodyL2)
    {
      auto splitEdge = SplitEdge(L2->getHeader(), bb, &DT, &LI);
      bb->replaceAllUsesWith(L2->getLoopLatch());
      bb->moveAfter(lastBasicBlockBodyL1);

      terminatorBodyL1->setOperand(0, bb);

      for (auto &inst : bb->getInstList())
      {
        if (inst.isTerminator())
        {
          inst.setOperand(0, L1->getLoopLatch());
          return;
        }
      }
    }
  }
  SmallVector<BasicBlock *> getBodyBlocks(Loop *L)
  {
    SmallVector<BasicBlock *> bodyBlocks;

    for (auto &bb : L->getBlocks())
    {
      if (bb != L->getHeader() && bb != L->getLoopLatch())
        bodyBlocks.push_back(bb);
    }
    return bodyBlocks;
  }
  void switchInductionVariables(Loop *L1, Loop *L2)
  {
    // Replace the induction variable of L2 with the induction variable of L1
    Value *inductionVariableL1 = &L1->getHeader()->getInstList().front();
    Instruction *inductionVariableL2 = &L2->getHeader()->getInstList().front();
    inductionVariableL2->replaceAllUsesWith(inductionVariableL1);
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
  bool haveCanonicalInductionVariable(Loop *L1, Loop *L2)
  {
    PHINode *inductionVariableL1 = L1->getCanonicalInductionVariable();
    PHINode *inductionVariableL2 = L2->getCanonicalInductionVariable();

    return inductionVariableL1 && inductionVariableL2;
  }
  bool haveControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT)
  {
    return DT.dominates(L1->getExitBlock(), L2->getLoopPreheader()) && PDT.dominates(L2->getLoopPreheader(), L1->getExitBlock());
  }
} // namespace
