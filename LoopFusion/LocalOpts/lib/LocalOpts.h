#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>


class LoopFusionPass final : public llvm::PassInfoMixin<LoopFusionPass> 
{
	public:
  		llvm::PreservedAnalyses run([[maybe_unused]] llvm::Function &, llvm::FunctionAnalysisManager &);
};

