#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include "LoopInvariantInstrAnalysis.hpp"

using namespace llvm;

/**
 * @returns Whether the instruction is loop invariant, using the cached information from the last pass.
*/
bool LoopInvariantInstrAnalysisPass::isLoopInvariant(const Instruction* instr) const {
  // Both methods boil down to SmallPtrSet.contains
  return
    invariants.contains(instr) // if we marked it as loop invariant
    || !invariants_loop->contains(instr); // or if it is not inside the loop
}

/**
 * @returns Whether the value is loop invariant, using the cached information from the last pass.
*/
bool LoopInvariantInstrAnalysisPass::isLoopInvariant(const Value* value) const {
  const Instruction* instr = dyn_cast<const Instruction>(value);
  if (instr == nullptr) {
    // Anything that is not an instruction (Constants) is loop invariant
    return true;
  }

  return isLoopInvariant(instr);
}


/** @returns Whether an instruction is loop invariant, assuming it is inside the loop */
bool LoopInvariantInstrAnalysisPass::isLoopInstructionLoopInvariant(const Instruction* instr) {
  /*
    Some instructions have additional implicit parameters or have side effects:
    this means that even if all the explicit parameters of an instructions are loop invariant,
    the instruction may not be considered loop invariant.

    Some examples:
    - the result of a phi instruction implicitly depends
      on the program counter, which is obviously not loop invariant.
    - a branch instruction has side effects on the program counter.
    - a memory store has side effects on memory.

    LLVM actually already has isSafeToSpeculativelyExecute,
    which doesn't check for memory reads or exception landing pads

    From lib/Analysis/LoopInfo.cpp, Loop::makeLoopInvariant
    if (!isSafeToSpeculativelyExecute(I))
      return false;
    if (I->mayReadFromMemory())
      return false;
    if (I->isEHPad())
      return false;

    Because i don't think i can use the above for the assignment,
    i'll just roll out my own (extremely restrictive) version.
  */
  switch (instr->getOpcode()) {
    case Instruction::Add:
    case Instruction::Sub:
    case Instruction::Mul:
    case Instruction::SDiv:
    case Instruction::UDiv:
    case Instruction::LShr:
    case Instruction::AShr:
    case Instruction::Shl:
      break;
    
    default:
      return false;
  }

  return all_of(instr->operands(),

    [this] (Value* val) { 
      return this->isLoopInvariant(val); 
    }
  ); 
}


/** @brief Computes isLoopInvariant for each instruction in the basic block and fills the cache. */
void LoopInvariantInstrAnalysisPass::runOnBasicBlock(const BasicBlock* bb) {
  for (const Instruction& instr : *bb) {
    // If the instruction should be considered loop invariant, then put it in the cache
    if (isLoopInstructionLoopInvariant(&instr))
      invariants.insert(&instr);
  }
}


void LoopInvariantInstrAnalysisPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}


bool LoopInvariantInstrAnalysisPass::runOnLoop(Loop *L, LPPassManager &LPM) {
  outs() << "\n BEGINNING LOOP INVARIANT INSTRUCTION ANALYSIS...\n";
  
  invariants_loop = L;
  invariants.clear();

  if (!L->isLoopSimplifyForm())
    return false;

  // Repeat until convergence
  decltype(invariants)::size_type old_size;
  do {
    old_size = invariants.size();

    for (const BasicBlock* bb : L->blocks())
      runOnBasicBlock(bb);

  } while (old_size != invariants.size());

  return false; 
}

char LoopInvariantInstrAnalysisPass::ID = 1;
static RegisterPass<LoopInvariantInstrAnalysisPass> X("loop-invariant-instr-analysis",
                             "Loop Invariant Instruction Analysis");