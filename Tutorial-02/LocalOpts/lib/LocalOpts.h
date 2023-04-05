#ifndef LOCALOPTS_H
#define LOCALOPTS_H

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public:
    llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);
};

class AlgebricIdentityPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public:
    llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);
}; 

class StrengthReductionPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public:
    llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);
}; 

class MultiInstructionPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public:
    llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);
}; 

#endif