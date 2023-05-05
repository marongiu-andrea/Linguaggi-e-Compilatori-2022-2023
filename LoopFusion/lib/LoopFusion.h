#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>

class LoopFusion : public llvm::PassInfoMixin<LoopFusion>
{
  public:
    llvm::PreservedAnalyses run(llvm::Function&, llvm::FunctionAnalysisManager&);
};