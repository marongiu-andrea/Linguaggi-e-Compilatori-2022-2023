#pragma once // NOLINT(llvm-header-guard)

#include <concepts>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>

class LoopFusion : public llvm::PassInfoMixin<LoopFusion>
{
  private:
    bool areLoopsAdjacent(const llvm::Loop*, const llvm::Loop*) const;

    void findAdjacentLoops(const std::vector<llvm::Loop*>&) const;

  public:
    llvm::PreservedAnalyses run(llvm::Function&, llvm::FunctionAnalysisManager&);
};