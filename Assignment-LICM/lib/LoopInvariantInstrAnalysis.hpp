#ifndef LOOP_INVARIANT_INSTR_ANALYSIS_HPP
#define LOOP_INVARIANT_INSTR_ANALYSIS_HPP

using namespace llvm;

class LoopInvariantInstrAnalysisPass final : public LoopPass {
private:
  /** @brief The cached information about the loop-invariant instructions 
   *         contained in the loop the pass was last executed on. */
  SmallPtrSet<const Instruction*, 32> invariants;

  /** @brief The loop on which the pass was last executed on. */
  Loop* invariants_loop;

  /** @returns Whether an instruction is loop invariant, assuming it is inside the loop */
  bool isLoopInstructionLoopInvariant(const Instruction* instr);

  void runOnBasicBlock(const BasicBlock* bb);

public:
  static char ID;

  LoopInvariantInstrAnalysisPass() : LoopPass(ID) {}
  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override;

  /**
  * @returns Whether the value is loop invariant, using the cached information from the last pass.
  */
  bool isLoopInvariant(const Value* value) const;

  /**
   * @returns Whether the instruction is loop invariant, using the cached information from the last pass.
  */
  bool isLoopInvariant(const Instruction* instr) const;
};

#endif // LOOP_INVARIANT_INSTR_ANALYSIS_HPP