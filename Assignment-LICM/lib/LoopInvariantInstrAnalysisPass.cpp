#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

class LoopInvariantInstrAnalysisPass final : public LoopPass {
public:
  static char ID;

  LoopInvariantInstrAnalysisPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {

  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\n BEGINNING LOOP INVARIANT INSTRUCTION ANALYSIS...\n"; 
    return false; 
  }
};

char LoopInvariantInstrAnalysisPass::ID = 1;
static RegisterPass<LoopInvariantInstrAnalysisPass> X("loop-invariant-instr-analysis",
                             "Loop Invariant Instruction Analysis");