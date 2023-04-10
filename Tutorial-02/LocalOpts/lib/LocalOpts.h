#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; 

class MultiInstructionPass final
    : public llvm::PassInfoMixin<MultiInstructionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
};