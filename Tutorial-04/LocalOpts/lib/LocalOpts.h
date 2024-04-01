#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constants.h>
#include <llvm/Pass.h>
#include <llvm/ADT/SmallVector.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

class TransformPass final : public PassInfoMixin<TransformPass> 
{
  public : 
  static char ID;
  PreservedAnalyses run(Function &, FunctionAnalysisManager &);
  private:
  void getAnalysisUsage(llvm::AnalysisUsage &AU);
  bool isPreHeaderExitBlock(BasicBlock* block1, BasicBlock* block2);
  bool equalTripCount(Loop *L1, Loop *L2, ScalarEvolution &SE);
  bool controlFlowEq(BasicBlock* block1, BasicBlock* block2, DominatorTree &DT, PostDominatorTree &PDT );
}; 

