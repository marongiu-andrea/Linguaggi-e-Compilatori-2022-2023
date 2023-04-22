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

  /**
   * @returns Whether the instruction has been marked as loop invariant during the last pass,
   *        or whether the instruction is not contained in the loop on which the pass was last executed.
  */
  bool isLoopInvariant(const Instruction* instr) const;
};

#endif // LOOP_INVARIANT_INSTR_ANALYSIS_HPP