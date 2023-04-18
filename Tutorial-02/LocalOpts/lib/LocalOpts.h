#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/Constants.h>
#include <llvm/IR/PassManager.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
};

class StrengthReductionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
};

class MultiInstructionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
};
