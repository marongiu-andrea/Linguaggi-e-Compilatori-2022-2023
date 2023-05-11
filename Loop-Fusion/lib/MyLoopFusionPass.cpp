#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"

using namespace llvm;

bool are_adjacent(Loop* l1, Loop* l2) {
  llvm::BasicBlock* l1_exit = l1->getUniqueExitBlock();
  llvm::BasicBlock* l2_pre = l2->getLoopPreheader();
  
  // If L1 has only one exit block and it is equal to the preheader of L2
  if (l1_exit == nullptr || l2_pre == nullptr || l1_exit != l2_pre)
    return false;  

  llvm::BasicBlock* bb = l2_pre;

  /* There must be only one instruction in bb
      and that instruction must be a branch to the header of l2.

    Because bb is the preheader of l2, then the second condition is guaranteed.
  */
  assert(bb->getTerminator()->getOpcode() == Instruction::Br);
  return bb->size() == 1;
}


typedef SmallVector<Loop*> CfeSet;

void runOnCfeSet(CfeSet cfe_set) {

}

PreservedAnalyses MyLoopFusionPass::run(Function &F,
                                             FunctionAnalysisManager & AM) {
  outs() << "BEGINNING MyLoopFusionPass...\n";

  const LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  const DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  const PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);

  SmallVector<CfeSet> cfe_sets;
  
  for (Loop* loop : LI) {
    if (!loop->isLoopSimplifyForm())
      continue;

    BasicBlock* loopPreheader = loop->getLoopPreheader();

    // Find a set with which loop is CFE
    CfeSet* set = std::find_if(cfe_sets.begin(), cfe_sets.end(),
      [loopPreheader, &DT, &PDT](CfeSet set) {
        /* If any of the loops contained in the CFE set 
            is dominated by loop AND post-dominates loop (or viceversa),
            then loop is CFE with the set.
        */
        BasicBlock* l_preheader = (*set.begin())->getLoopPreheader();

        auto dom_condition = [&DT, &PDT](BasicBlock* a, BasicBlock* b) {
          return DT.dominates(a, b)
            && PDT.dominates(a, b);  
        };

        return dom_condition(loopPreheader, l_preheader)
            || dom_condition(l_preheader, loopPreheader);
      }
    );

    if (set) {
      // If there is a set with which loop is CFE, then add loop to that set
      set->push_back(loop);

    } else {
      // Otherwise, create a new CFE set
      cfe_sets.push_back(CfeSet { loop });
    }
  }

  for (CfeSet& cfe_set : cfe_sets) {
    if (cfe_set.size() <= 1)
      continue;
    
    // sort the CFE set by dominance
    std::sort(cfe_set.begin(), cfe_set.end(), [&DT] (Loop* a, Loop* b) {
      return DT.dominates(a->getLoopPreheader(), b->getLoopPreheader());
    });

    runOnCfeSet(cfe_set);
  }

  return PreservedAnalyses::none();
}