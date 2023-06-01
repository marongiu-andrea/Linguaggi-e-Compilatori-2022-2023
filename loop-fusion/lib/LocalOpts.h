#ifndef LOCALOPTS_H
#define LOCALOPTS_H

#include <llvm/IR/PassManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/PostDominators.h>


using namespace llvm;

class LoopFusionPass final : public PassInfoMixin<LoopFusionPass> 
{
  bool areAdjacent(Loop*, Loop*);
  bool sameTripCount(Loop*, Loop*, ScalarEvolution&);
  bool sameGuardBranch(Loop*, Loop*);
  bool controlFlowEquivalence(Loop*, Loop*, DominatorTree&, PostDominatorTree&);
  bool haveNegativeDistanceDependence(Loop*, Loop*);

  void mergeLoops(Loop*, Loop*, ScalarEvolution&);

  public:
    PreservedAnalyses run(Function&, FunctionAnalysisManager&);
};

#endif