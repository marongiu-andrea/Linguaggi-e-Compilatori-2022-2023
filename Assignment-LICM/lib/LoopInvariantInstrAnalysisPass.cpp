#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

class LoopInvariantInstrAnalysisPass final : public LoopPass {
private:
  /** @brief The cached information about the loop-invariant instructions 
   *         contained in the loop the pass was last executed on. */
  SmallPtrSet<const Instruction*, 32> invariants;

  /** @brief The loop on which the pass was last executed on. */
  Loop* invariants_loop;

  bool computeIsLoopInvariant(const Instruction* instr) {
    // An instruction is loop invariant if all its operands are loop invariant.    
    return all_of(instr->operand_values(),

      [this] (const Value* op) {
        const Instruction* op_instr = dyn_cast<Instruction>(op);
        if (op_instr == nullptr)
          // Anything that is not an instruction (Constants) is loop invariant
          return true;

        // Instructions must either be located outside the loop or we must have already marked them as loop invariant.
        return this->isLoopInvariant(op_instr);
      }

    );
  }

  void runOnBasicBlock(const BasicBlock* bb) {
    for (const Instruction& instr : *bb) {
      // If the instruction is already marked as loop invariant, skip
      if (invariants.contains(&instr))
        continue;

      // If the instruction should be considered loop invariant, then do so
      if (computeIsLoopInvariant(&instr))
        invariants.insert(&instr);
    }
  }

public:
  static char ID;

  LoopInvariantInstrAnalysisPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\n BEGINNING LOOP INVARIANT INSTRUCTION ANALYSIS...\n";
    
    if (!L->isLoopSimplifyForm())
      return false;

    invariants_loop = L;
    invariants.clear();

    // Repeat until convergence
    decltype(invariants)::size_type old_size;
    do {
      old_size = invariants.size();

      for (const BasicBlock* bb : L->blocks())
        runOnBasicBlock(bb);

    } while (old_size != invariants.size());

    return false; 
  }

  /**
   * @returns Whether the instruction has been marked as loop invariant during the last pass,
   *        or whether the instruction is not contained in the loop on which the pass was last executed.
  */
  bool isLoopInvariant(const Instruction* instr) {
    // Both methods boil down to SmallPtrSet.contains
    return
      invariants.contains(instr) // if we marked it as loop invariant
      || !invariants_loop->contains(instr); // or if it is not inside the loop
  }
};

char LoopInvariantInstrAnalysisPass::ID = 1;
static RegisterPass<LoopInvariantInstrAnalysisPass> X("loop-invariant-instr-analysis",
                             "Loop Invariant Instruction Analysis");