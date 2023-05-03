#ifndef locaOpts_H
#define locaOpts_H

#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>

using namespace llvm;

/*Definizione classe TransformPass*/
class TransformPass final : public llvm::PassInfoMixin<TransformPass> {
  public: // llvm::Preser...
    PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass


/*Definizione classe AlgebraicIdentity*/
class AlgebraicIdentity final : public llvm::PassInfoMixin<AlgebraicIdentity> {
  public: // llvm::Preser...
    PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

/*Definizione classe Strength Reduction*/
class Sred final : public llvm::PassInfoMixin<Sred> {
  public: // llvm::Preser...
    PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass

/*Definizione classe Multi Instruction Optimization*/
class MultiInstruction final : public llvm::PassInfoMixin<MultiInstruction> {
  public: // llvm::Preser...
    PreservedAnalyses run(llvm::Module &, llvm::ModuleAnalysisManager &);
}; // class AlgebraicIdentityPass


#endif
