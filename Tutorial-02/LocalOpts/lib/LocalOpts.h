#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>


class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
  bool runOnFunction(llvm::Function &F);
  bool runOnBasicBlock(llvm::BasicBlock &BB);
};

class StrengthReductionPass final : public llvm::PassInfoMixin<StrengthReductionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
  bool runOnFunction(llvm::Function &F);
  bool runOnBasicBlock(llvm::BasicBlock &BB);
};

class MIOptimizationPass final : public llvm::PassInfoMixin<MIOptimizationPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
  bool runOnFunction(llvm::Function &F);
  bool runOnBasicBlock(llvm::BasicBlock &BB);
};