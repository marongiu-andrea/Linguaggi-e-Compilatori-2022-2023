#ifndef LOOP_INVARIANT_INSTR_ANALYSIS_HPP
#define LOOP_INVARIANT_INSTR_ANALYSIS_HPP

//#include <llvm/Analysis/LoopPass.h>

using namespace llvm;

class LoopInvariantInstrAnalysisPass final : public LoopPass {
public:
  static char ID;

  LoopInvariantInstrAnalysisPass();
  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override;
};

#endif // LOOP_INVARIANT_INSTR_ANALYSIS_HPP