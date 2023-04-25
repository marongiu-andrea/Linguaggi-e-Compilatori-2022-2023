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
    ArrayRef<BasicBlock*> loopExits,
    Instruction* inst,
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

    BasicBlock* preheader = L->getLoopPreheader();

    bool allOperandsOutsideLoop = all_of(inst->operands(), [L, preheader] (Value* val) {

      /* The data used by llvm::Loop.contains is not updated when moving instructions.

        Because we only move instructions to the preheader, an instruction is outside the loop if:
        - It is in the preheader, which is outside the loop
        - OR Loop.contains returns false (it was never located in the loop to begin with)
      */
   
      Instruction* instr = dyn_cast<Instruction>(val);
      return instr == nullptr || instr->getParent() == preheader || !L->contains(instr);
    });

    if (!allOperandsOutsideLoop)
      return false;


    bool dominatesAllExits = all_of(loopExits, [DT, inst] (BasicBlock* exit) { return DT->dominates(inst, exit); });
    
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

    if (!(dominatesAllExits || valueIsDeadOutsideLoop))
      return false;

    return true;
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

    // Traverse in depth-first pre-order.
    // Repeating the same block twice is not useful because we're in SSA, so we can use a naive algorithm.
    BasicBlock* header = L->getHeader();
    SmallPtrSet<BasicBlock*, 16> visited = { header };
    SmallVector<BasicBlock*> stack = { header };

    // Guaranteed to be there because the loop is in normal/natural/simplify form
    BasicBlock* preheader = L->getLoopPreheader();

    bool changed = false;
    while (stack.size() > 0) {
      BasicBlock* bb = stack.pop_back_val();

      auto ii = bb->begin();
      auto end = bb->end();
      while (ii != end) {
        Instruction* instr = &*ii;
        
        ++ii; // Increment it before we modify the instruction

        if (shouldInstructionBeMoved(L, exitBlocks, instr, DT, LII)) {
          changed = true;

          outs() << "Moving ";
          instr->print(outs());
          outs() << "\n";

          // The last instruction of a basic block is a branch instruction, so we move before that
          instr->moveBefore(&preheader->back());

          if (ii != end) {
            Instruction* next = &*ii;
            ii = BasicBlock::iterator(next);
          }
        }
      }
      
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

