#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

class MultiInstructionPass final
    : public llvm::PassInfoMixin<MultiInstructionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; // class MultiInstructionPass

class StrengthReductionPass final
    : public llvm::PassInfoMixin<MultiInstructionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; // class StrengthReductionPass
