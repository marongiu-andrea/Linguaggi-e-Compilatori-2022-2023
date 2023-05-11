#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/Constants.h>
#include <llvm/IR/PassManager.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &, llvm::FunctionAnalysisManager &);
}; // class AlgebraicIdentityPass