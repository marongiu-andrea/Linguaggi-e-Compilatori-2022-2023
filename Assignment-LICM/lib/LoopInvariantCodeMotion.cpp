#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

#include "LoopInvariantInstrAnalysis.hpp"


using namespace llvm;

namespace {

class LoopInvariantCodeMotion final : public LoopPass {
private:
  bool runOnInstruction(
    Loop* L,
    ArrayRef<BasicBlock*> loopExits,
    Instruction* inst,
    const DominatorTree* DT,
    const LoopInvariantInstrAnalysisPass* LII)
  {
    /* 
    An instruction can be moved IF:
    !- It is loop invariant
    !- It dominates all loop exits

    - There is no other instruction which assigns to the same LHS (Already guaranteed by SSA)
    - It dominates all its uses (Already guaranteed by SSA)
    */


    if (!(
        all_of(loopExits, [DT, inst] (BasicBlock* exit) { return DT->dominates(inst, exit); }) 
        && LII->isLoopInvariant(inst)
       ))
      return false;

    inst->print(outs());
    outs() << "\n";
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

    bool changed = false;
    while (stack.size() > 0) {
      BasicBlock* bb = stack.pop_back_val();

      for (auto ii = bb->begin(); ii != bb->end(); ++ii) {
        changed = runOnInstruction(L, exitBlocks, &*ii, DT, LII) || changed;
      }

      for (BasicBlock* bb : successors(bb)) {
        if (!visited.contains(bb)) {
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

