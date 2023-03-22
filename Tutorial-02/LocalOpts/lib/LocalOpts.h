#ifndef LOCALOPTS_H
#define LOCALOPTS_H

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

class TransformPass final : public llvm::PassInfoMixin<TransformPass> 
{
  public:
    llvm::PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);


}; // class AlgebraicIdentityPass

#endif