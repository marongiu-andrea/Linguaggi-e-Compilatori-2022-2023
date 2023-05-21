#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/IR/Dominators.h"
#include <llvm/IR/Function.h>
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include <llvm/IR/PassManager.h>

using namespace llvm;

namespace
{
  bool areLoopsAdjacent(Loop *L1, Loop *L2, LoopInfo &LI);
  bool haveLoopsSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE);

  class LoopFusion final : public FunctionPass
  {
  public:
    static char ID;

    LoopFusion() : FunctionPass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<ScalarEvolutionWrapperPass>();
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.setPreservesAll();
    }
    bool runOnLoop(Loop *L)
    {
      outs() << "\nLOOPPASS INIZIATO...\n";
      if (L->isLoopSimplifyForm())
      {
        outs() << "FORMA NORMALIZZATA\n";
      }
      BasicBlock *BB = L->getLoopPreheader();
      if (BB)
      {
        outs() << "PREHEADER " << *BB << "\n";
      }
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
      {
        BasicBlock *B = *BI;

        outs() << "Basic Block: ";
        B->printAsOperand(outs(), false);
        outs() << "\n";

        for (auto Iter = B->begin(); Iter != B->end(); ++Iter)
        {
          outs() << *Iter << "\n";
        }
      }

      return true;
    }

    virtual bool runOnFunction(Function &F) override
    {
      outs() << "\n\n";
      bool Transformed = false;
      LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
      ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();

      Loop *nextLoop = nullptr;
      for (Loop *L : LI)
      {
        if (nextLoop)
        {
          outs() << "LOOP predecessor: " << *nextLoop << "\n";
          outs() << "LOOP: " << *L << "\n";
          if (areLoopsAdjacent(L, nextLoop, LI))
          {
            outs() << "LOOPS is adjacent\n";
            if (haveLoopsSameTripCount(L, nextLoop, SE))
            {
              outs() << "LOOPS have same trip count\n";
            }
          }
        }
        nextLoop = L;
      }

      return Transformed;
    }
  };

  char LoopFusion::ID = 0;
  RegisterPass<LoopFusion> X("loop-fusion-pass", "Loop Fusion Pass");

  bool areLoopsAdjacent(Loop *L1, Loop *L2, LoopInfo &LI)
  {
    return L1->getExitBlock() == L2->getLoopPreheader();
  }
  bool haveLoopsSameTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE)
  {

    const SCEV *TripCount1 = SE.getBackedgeTakenCount(L1);
    const SCEV *TripCount2 = SE.getBackedgeTakenCount(L2);

    outs() << "LOOP1: \n";
    TripCount1->dump();
    Optional<uint64_t> ConstantTripCount = SE.getSmallConstantTripCount(L1);
    if (ConstantTripCount.hasValue())
    {
      outs() << "Constant Trip Count: " << ConstantTripCount.getValue() << "\n";
    }
    else
    {
      outs() << "Trip Count is not a small constant\n";
    }
    outs() << "\tTrip count SCEVType: " << TripCount1->getSCEVType() << "\n";

    outs() << "LOOP2: \n";
    TripCount2->dump();
    outs() << "\tTrip count SCEVType: " << TripCount2->getSCEVType() << "\n";

    bool SameTripCount = SE.isKnownPredicate(ICmpInst::ICMP_EQ, TripCount1, TripCount2);
    outs() << "SameTripCount: " << SameTripCount << "\n";

    return SameTripCount;
  }
} // namespace
