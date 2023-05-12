#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include <llvm/Pass.h>

class LoopFusionPass final : public llvm::ModulePass {
    public:
    static char ID;
    
    LoopFusionPass() : llvm::ModulePass(LoopFusionPass::ID) {}
    
    virtual bool runOnModule(llvm::Module &) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage &AU) const override;

    bool runOnFunction(llvm::Function &F);
}; 
