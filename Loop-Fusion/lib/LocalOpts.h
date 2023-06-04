#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class MyLoopFusionPass final
    : public llvm::PassInfoMixin<MyLoopFusionPass> {
public:
  llvm::PreservedAnalyses run(llvm::Function &,
                              llvm::FunctionAnalysisManager &);
};