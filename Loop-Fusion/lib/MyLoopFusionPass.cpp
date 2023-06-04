#include "LocalOpts.h"

#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Dominators.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"

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

bool have_identical_trip_count(Loop* a, Loop* b, ScalarEvolution& SE) {
  const SCEV* a_tripCount = SE.getBackedgeTakenCount(a);
  if (isa<SCEVCouldNotCompute>(a_tripCount))
    return false;

  const SCEV* b_tripCount = SE.getBackedgeTakenCount(b);
  if (isa<SCEVCouldNotCompute>(b_tripCount))
    return false;

  return a_tripCount == b_tripCount;
}

bool have_negative_distance_dependencies(Loop* a, Loop* b, ScalarEvolution& SE) {
  return true;
}

typedef SmallVector<Loop*> CfeSet; 
BasicBlock* getLoopBodyBlock(Loop* a){
  BasicBlock* header = a->getHeader();
  BranchInst* inst = dyn_cast<BranchInst>(header->getTerminator());
  return inst->getSuccessor(0);

}
void replaceUses(Loop * a, Loop * b ) {
    dyn_cast<PHINode>(b->getHeader()->begin())->replaceAllUsesWith(dyn_cast<PHINode>(a->getHeader()->begin()));
    dyn_cast<PHINode>(b->getHeader()->begin())->eraseFromParent(); 
  }

void fuse(Loop* a, Loop* b) {
  BasicBlock* body_a = getLoopBodyBlock(a);
  BasicBlock* body_b = getLoopBodyBlock(b);
  BasicBlock* header_a = a->getHeader();
  BasicBlock* header_B = b->getHeader();
  BasicBlock* exitingBlock_b = b->getExitingBlock(); 
  BasicBlock* latch_a = a->getLoopLatch();
  BranchInst* last_inst;
  outs() << "Fusing ";
  a->getLoopPreheader()->printAsOperand(outs());
  outs() << " with ";
  b->getLoopPreheader()->printAsOperand(outs());
  outs() << "\n";
  replaceUses(a,b);
  /*
    attach the body of loop a with loop b
  */
  last_inst = dyn_cast<BranchInst>(body_a->getTerminator());
  last_inst->setSuccessor(0,body_b);
  /*
    attach header of loop a with loop b
  */
  last_inst = dyn_cast<BranchInst>(header_a->getTerminator());
  last_inst->setSuccessor(1,exitingBlock_b);
  /*
    attact the body of loop b with loop a to complete fusion
  */
  last_inst = dyn_cast<BranchInst>(body_b->getTerminator());
  last_inst->setSuccessor(0,latch_a);


}


void runOnCfeLoopPair(Loop* a, Loop* b, ScalarEvolution& SE) {
  if (!are_adjacent(a, b))
    return;

  if (!have_identical_trip_count(a, b, SE))
    return;

  auto iv_a = a->getCanonicalInductionVariable();
  if (iv_a == nullptr)
    return;
  
  auto iv_b = b->getCanonicalInductionVariable();
  if (iv_b == nullptr)
    return;
  
  /* iv_a and iv_b are canonical (they start from 0 and are incremented by one)
      and the two loops have the same trip count
      => iv_a and iv_b have the same range.
  */

 /* if (have_negative_distance_dependencies(a, b))
    return;
*/
  fuse(a, b);
}

void runOnCfeSet(CfeSet& cfe_set, ScalarEvolution& SE) {
  const int N = cfe_set.size();
  for (int i = 0; i < N; ++i) {
    for (int j = i + 1; j < N; ++j) {
      runOnCfeLoopPair(cfe_set[i], cfe_set[j], SE);
    }
  }
}

PreservedAnalyses MyLoopFusionPass::run(Function &F,
                                             FunctionAnalysisManager & AM) {
  outs() << "BEGINNING MyLoopFusionPass...\n";

  const LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  const DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  const PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

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
            && PDT.dominates(b, a); 
        };

        return dom_condition(loopPreheader, l_preheader)
            || dom_condition(l_preheader, loopPreheader);
      }
    );

    
    if (set != cfe_sets.end()) {

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

    runOnCfeSet(cfe_set, SE);
  }

  return PreservedAnalyses::none();
}