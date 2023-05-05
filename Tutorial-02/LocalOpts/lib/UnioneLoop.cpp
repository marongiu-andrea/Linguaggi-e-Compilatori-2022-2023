#include "LocalOpts.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopPass.h"
/*
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
*/


using namespace llvm;


PreservedAnalyses UnioneLoopPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &AM) {

    auto &LI = AM.getResult<LoopAnalysis>(F);

    /*AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();*/


  return PreservedAnalyses::none();
}

