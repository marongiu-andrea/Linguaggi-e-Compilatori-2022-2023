#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/PassManager.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopAnalysisManager.h"

using namespace llvm;

PreservedAnalyses Mem2RegPass::run([[maybe_unused]] /*Module*/ Function &F,
                                             FunctionAnalysisManager &AM) {

  // Un semplice passo di esempio di manipolazione della IR
  auto &LI = AM.getResult<LoopAnalysis>(F);

  return PreservedAnalyses::none();
}

