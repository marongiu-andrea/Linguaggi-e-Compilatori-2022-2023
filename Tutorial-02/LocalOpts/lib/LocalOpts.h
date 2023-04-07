#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final
    : public llvm::PassInfoMixin<TransformPass> {
public:
  llvm::PreservedAnalyses run(llvm::Module &,
                              llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

class AlgebraicPass final:
	public llvm::PassInfoMixin<AlgebraicPass> {
		public: llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
	};

class StrengthPass final:
	public llvm::PassInfoMixin<StrengthPass> {
		public: llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
	};

class MultiplePass final:
	public llvm::PassInfoMixin<MultiplePass> {
		public: llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
	};