#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopInvariantCodeMotion final : public LoopPass {
public:
  static char ID;

  LoopInvariantCodeMotion() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\n BEGINNING LOOP INVARIANT CODE MOTION PASS...\n"; 
    return false; 
  }
};

char LoopInvariantCodeMotion::ID = 0;
RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                             "Loop Invariant Code Motion");

} // anonymous namespace

