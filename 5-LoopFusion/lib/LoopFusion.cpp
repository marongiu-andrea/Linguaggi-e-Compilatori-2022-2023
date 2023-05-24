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

          outs() << "\tLOOPS are fusionable\n";

          loopFusion(L, nextLoop, DT, LI);
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

    SmallVector<BasicBlock *> blocksToMove;
    for (auto &bb : L2->getBlocks())
    {
      if (bb == L2->getHeader())
        continue;

      if (bb == L2->getLoopLatch())
        continue;

      blocksToMove.push_back(bb);
    }

    // Replace the induction variable of L2 with the induction variable of L1
    Value *inductionVariableL1 = &L1->getHeader()->getInstList().front();
    Instruction *inductionVariableL2 = &L2->getHeader()->getInstList().front();
    inductionVariableL2->replaceAllUsesWith(inductionVariableL1);

    BasicBlock *L1Body = nullptr;
    for (auto &bb : L1->getBlocks())
    {
      if (bb == L1->getHeader())
        continue;
      L1Body = bb;
      break;
    }

    Instruction *L1Terminator = nullptr;
    for (auto &inst : L1Body->getInstList())
    {
      if (inst.isTerminator())
      {
        L1Terminator = &inst;
        break;
      }
    }

    // TODO: Move the body of L2 next to the body of L1

    for (auto &bb : blocksToMove)
    {
      auto splitEdge = SplitEdge(L2->getHeader(), bb, &DT, &LI);
      bb->replaceAllUsesWith(L2->getLoopLatch());
      bb->moveAfter(L1Body);

      L1Terminator->setOperand(0, bb);

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
