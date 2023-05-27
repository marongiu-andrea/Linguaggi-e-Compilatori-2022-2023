#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public : llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; 

// class IdentityPass
class IdentityPass final : public llvm::PassInfoMixin<IdentityPass> 
{
  public : llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; 

// class StrengthPass
class StrengthPass final : public llvm::PassInfoMixin<StrengthPass> 
{
  public : llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; 

// class MultiPass
class MultiPass final : public llvm::PassInfoMixin<MultiPass> 
{
  public : llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; 

