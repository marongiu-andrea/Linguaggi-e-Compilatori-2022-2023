#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID)
  {
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override 
  { 
    AU.setPreservesAll();
  }

  virtual bool runOnLoop(Loop* L, LPPassManager& LPM) override
  {
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-invariant-code-motion","Loop Invariant Code Motion");

} // anonymous namespace

