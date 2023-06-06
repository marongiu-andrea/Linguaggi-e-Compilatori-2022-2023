#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>

class LocalOpts
{
  private:
    bool runOnFunction(llvm::Function&);

  protected:
    virtual bool runOnBasicBlock(llvm::BasicBlock&) = 0;

  public:
    llvm::PreservedAnalyses run(llvm::Module&, llvm::ModuleAnalysisManager&);
};

class AlgebraicIdentityPass final : public llvm::PassInfoMixin<AlgebraicIdentityPass>, public LocalOpts
{
  protected:
    bool runOnBasicBlock(llvm::BasicBlock&) override;
};

class StrengthReductionPass final : public llvm::PassInfoMixin<StrengthReductionPass>, public LocalOpts
{
  protected:
    bool runOnBasicBlock(llvm::BasicBlock&) override;
};

class MultiInstrOptPass final : public llvm::PassInfoMixin<MultiInstrOptPass>, public LocalOpts
{
  protected:
    bool runOnBasicBlock(llvm::BasicBlock&) override;
};