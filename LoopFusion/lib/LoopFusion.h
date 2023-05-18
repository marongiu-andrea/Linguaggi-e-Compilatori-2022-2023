#pragma once // NOLINT(llvm-header-guard)

#include <concepts>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Transforms/Scalar/LoopRotation.h>

class LoopFusion : public llvm::PassInfoMixin<LoopFusion>
{
  private:
    bool areLoopsAdjacent(const llvm::Loop*, const llvm::Loop*) const;

    void findAdjacentLoops(const std::vector<llvm::Loop*>& loops, std::vector<std::pair<llvm::Loop*, llvm::Loop*>>& adjLoopPairs) const;
    bool haveSameTripCount(llvm::ScalarEvolution& SE, llvm::Loop const* loopi, llvm::Loop const* loopj) const;
    bool checkControlFlowEquivalence(llvm::DominatorTree& DT, llvm::PostDominatorTree& PT, const llvm::Loop* loopi, const llvm::Loop* loopj) const;
    bool checkNegativeDistanceDeps(llvm::Loop* loopi, llvm::Loop* loopj) const;
    bool mergeLoops(llvm::Loop* loopFused, llvm::Loop* loopToFuse, llvm::ScalarEvolution &SE, llvm::LoopInfo &LI) const;
    llvm::PHINode *getInductionVariable(llvm::Loop *L, llvm::ScalarEvolution &SE) const;

  public:
    llvm::PreservedAnalyses run(llvm::Function&, llvm::FunctionAnalysisManager&);
};
