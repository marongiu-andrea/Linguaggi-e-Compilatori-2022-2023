#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>


class LoopFusionPass final : public llvm::PassInfoMixin<LoopFusionPass> 
{
	public:
  		llvm::PreservedAnalyses run([[maybe_unused]] llvm::Function &, llvm::FunctionAnalysisManager &);
	private:
		bool areL1andL2Adjacent(llvm::BasicBlock * exitBlockL1, llvm::BasicBlock * preheaderL2, llvm::BasicBlock * headerL2);
};

