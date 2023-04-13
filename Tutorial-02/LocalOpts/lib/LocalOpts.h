#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};

class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
};

class StrengthReductionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &F,
                              llvm::FunctionAnalysisManager &);
};

class MultiInstructionOperationsPass final
    : public llvm::PassInfoMixin<MultiInstructionOperationsPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &F,
                              llvm::ModuleAnalysisManager &);
};