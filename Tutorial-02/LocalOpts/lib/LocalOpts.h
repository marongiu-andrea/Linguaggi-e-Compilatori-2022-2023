#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>


class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
  bool runOnFunction(llvm::Function &F);
  bool runOnBasicBlock(llvm::BasicBlock &BB);
};
class AlgebraicIdentityPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
  bool runOnFunction(llvm::Function &F);
  bool runOnBasicBlock(llvm::BasicBlock &BB);
};

