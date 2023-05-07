#pragma once

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include "llvm/Analysis/CGSCCPassManager.h"
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/PostDominators.h>
#include <llvm/Analysis/LoopPass.h>
#include "llvm/Analysis/LoopInfo.h"
#include <llvm/Analysis/ValueTracking.h>


class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
public:

  llvm::PreservedAnalyses run(
    [[maybe_unused]] llvm::Function &F,
    llvm::FunctionAnalysisManager &AM
    );
};