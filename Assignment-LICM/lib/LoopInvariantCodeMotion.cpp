#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include "LoopInvariantInstrAnalysis.hpp"

using namespace llvm;

namespace {

class LoopInvariantCodeMotion final : public LoopPass {
public:
  static char ID;

  LoopInvariantCodeMotion() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInvariantInstrAnalysisPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\n BEGINNING LOOP INVARIANT CODE MOTION PASS...\n"; 
    return false; 
  }
};

char LoopInvariantCodeMotion::ID = 0;
static RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                             "Loop Invariant Code Motion");

} // anonymous namespace

