#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class LoopFusionPass final
    : public llvm::PassInfoMixin<LoopFusionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &AM);
}; // class AlgebraicIdentityPass

