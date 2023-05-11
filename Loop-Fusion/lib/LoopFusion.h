#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include <llvm/Pass.h>

class LoopFusionPass final : public llvm::ModulePass {
    public:
    virtual bool runOnModule(llvm::Module &) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;
}; 
