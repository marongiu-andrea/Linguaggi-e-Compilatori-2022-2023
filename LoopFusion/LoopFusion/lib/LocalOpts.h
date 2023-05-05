#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class Mem2RegPass final
    : public llvm::PassInfoMixin<Mem2RegPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &AM);
}; // class AlgebraicIdentityPass

