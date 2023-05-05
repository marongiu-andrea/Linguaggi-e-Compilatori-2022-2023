#pragma once

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include "llvm/Analysis/CGSCCPassManager.h"


class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(
    [[maybe_unused]] llvm::Function &F,
    llvm::FunctionAnalysisManager &AM
    );
};