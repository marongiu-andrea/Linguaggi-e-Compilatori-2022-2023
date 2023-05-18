#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/IR/Dominators.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>

using namespace llvm;

namespace
{
  class LoopFusion final : public FunctionPass
  {
  public:
    static char ID;

    LoopFusion() : FunctionPass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.setPreservesAll();
    }
    bool runOnBasicBlock(BasicBlock &BB)
    {
      int BID = 0;
      BasicBlock *B = &BB;

      outs() << "Basic Block: ";
      B->printAsOperand(outs(), false);
      outs() << "\n";

      for (auto &I : BB)
      {
        outs() << "\t" << I << "\n";
      }
      return true;
    }

    virtual bool runOnFunction(Function &F) override
    {
      bool Transformed = false;
      DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
      for (auto &BB : F)
      {
        if (runOnBasicBlock(BB))
        {
          Transformed = true;
        }
      }
      return Transformed;
    }
  };

  char LoopFusion::ID = 0;
  RegisterPass<LoopFusion> X("loop-fusion-pass", "Loop Fusion Pass");
} // namespace
