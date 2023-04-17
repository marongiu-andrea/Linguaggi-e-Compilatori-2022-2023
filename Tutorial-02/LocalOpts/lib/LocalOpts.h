#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};

// class AlgebraicIdentityPass
class AlgebraicIdentityPass final
    : public llvm::PassInfoMixin<AlgebraicIdentityPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};

// class StrengthReductionPass
class StrengthReductionPass final
    : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};

// class MultiOpPass
class MultiOpPass final
    : public llvm::PassInfoMixin<MultiOpPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};
