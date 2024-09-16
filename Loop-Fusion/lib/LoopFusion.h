#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include <llvm/Pass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/PostDominators.h>


class LoopFusionPass final : public llvm::PassInfoMixin<LoopFusionPass> {
    public:
    llvm::PreservedAnalyses run(llvm::Function &,
                                llvm::FunctionAnalysisManager &MM);

private:
    bool isLoopAdjacent(llvm::Loop *a, llvm::Loop *b);
    bool checkBounds(llvm::Loop *a, llvm::Loop *b, llvm::ScalarEvolution &sce);
    bool checkDominance(llvm::Loop *a, llvm::Loop *b, llvm::DominatorTree &dt, 
                        llvm::PostDominatorTree &pdt);
    void loopFusion(llvm::Loop *a, llvm::Loop *b, llvm::ScalarEvolution &sce);
}; 
