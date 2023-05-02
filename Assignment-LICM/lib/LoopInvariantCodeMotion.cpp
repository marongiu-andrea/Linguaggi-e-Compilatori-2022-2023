#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

#include "LoopInvariantInstrAnalysis.hpp"


using namespace llvm;

namespace {

class LoopInvariantCodeMotion final : public LoopPass {
private:
  bool shouldInstructionBeMoved(
    Loop* L,
    Instruction* inst,
    BasicBlock* preheader,
    const ArrayRef<BasicBlock*> loopExits,
    const DominatorTree* DT,
    const LoopInvariantInstrAnalysisPass* LII)
  {
    /* An instruction can be moved IF:
      - It is loop invariant
      - All the values it depends on are defined outside the loop (e.g. they already have been moved)
      - It dominates all loop exits OR the value it defines is dead outside the loop

      - There is no other instruction which assigns to the same LHS (Already guaranteed by SSA)
      - It dominates all its uses (Already guaranteed by SSA)
    */

    if (!LII->isLoopInvariant(inst))
      return false;


    bool allOperandsOutsideLoop = all_of(inst->operands(), [L, preheader] (Value* val) {
      /*
        Because we only move instructions to the preheader,
        an instruction is outside the loop if:

        - It was never located in the loop to begin with,
           so llvm::Loop.contains returns false

        - OR if it is in the preheader, where we could've moved it
           without updating the data used by llvm::Loop.contains.
      */
      Instruction* instr = dyn_cast<Instruction>(val);
      return instr == nullptr || instr->getParent() == preheader || !L->contains(instr);
    });

    if (!allOperandsOutsideLoop)
      return false;


    /* We're in SSA (all definitions dominate their uses), 
        so liveness analysis consists of just
        checking that the users list is not empty.
        
      If all the users are instructions contained in the loop,
      the value is dead outside the loop.
    */
    bool valueIsDeadOutsideLoop = all_of(inst->users(), [L] (User* user) {
      Instruction* instr = dyn_cast<Instruction>(user);
      return instr != nullptr && L->contains(instr);
    });

    bool dominatesAllExits = all_of(loopExits, [DT, inst] (BasicBlock* exit) { return DT->dominates(inst, exit); });

    if (!(dominatesAllExits || valueIsDeadOutsideLoop))
      return false;


    return true;
  }

  bool runOnLoopBasicBlock(
    Loop* L,
    BasicBlock* bb,
    BasicBlock* preheader,
    const ArrayRef<BasicBlock*> loopExits,
    const DominatorTree* DT,
    const LoopInvariantInstrAnalysisPass* LII)
  {
    bool changed = false;
    LoopInfo a;

    auto ii = bb->begin();
    auto end = bb->end();
    while (ii != end) {
      /* Store the current instruction, then
          advance the iterator before we do any modification
      */
      Instruction* instr = &*(ii++);

      if (shouldInstructionBeMoved(L, instr, preheader, loopExits, DT, LII)) {
        changed = true;

        outs() << "Moving ";
        instr->print(outs());
        outs() << "\n";

        /* For both of the following lines,
            note that the last instruction of a basic block
            is always a branching instruction.
        */

        // Move the instruction before the last basic block instruction
        instr->moveBefore(&preheader->back());

        /* Replace the old (invalid) iterator with
            a new one pointing to the next instruction.
            
           ii != end is guaranteed: if it was false, then the current instruction
            would be a branching instruction, which is not loop invariant
            so we shouldn't be attempting to move it in the first place
        */
        ii = BasicBlock::iterator(&*ii);
      }
    }

    return changed;
  }

public:
  static char ID;

  LoopInvariantCodeMotion() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<LoopInvariantInstrAnalysisPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\n BEGINNING LOOP INVARIANT CODE MOTION PASS...\n"; 

    if (!L->isLoopSimplifyForm())
      return false;

    const DominatorTree* DT = 
      &getAnalysis<DominatorTreeWrapperPass>()
      .getDomTree();

    const LoopInvariantInstrAnalysisPass* LII =
      &getAnalysis<LoopInvariantInstrAnalysisPass>();
    

    SmallVector<BasicBlock*> exitBlocks;
    L->getExitBlocks(exitBlocks);

    // Guaranteed to be there because the loop is in normal/natural/simplify form
    BasicBlock* preheader = L->getLoopPreheader();

    bool changed = false;

    /* Traverse the loop in depth-first pre-order.
       
       Repeating the same block twice is not useful because
        we're in SSA, where all definitions must dominate their uses,
        so we can use a simple algorithm.
    */
    BasicBlock* header = L->getHeader();
    SmallPtrSet<BasicBlock*, 16> visited = { header };
    SmallVector<BasicBlock*> stack = { header };
    while (stack.size() > 0) {
      BasicBlock* bb = stack.pop_back_val();

      changed =
        runOnLoopBasicBlock(L, bb, preheader, exitBlocks, DT, LII)
        || changed;
      
      // Add all successors which are in the loop and that we haven't already visited
      for (BasicBlock* bb : successors(bb)) {
        if (!visited.contains(bb) && L->contains(bb)) {
          stack.push_back(bb);
          visited.insert(bb);
        }
      }
    }

    return changed;
  }
};

char LoopInvariantCodeMotion::ID = 0;
static RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                             "Loop Invariant Code Motion");

} // anonymous namespace

