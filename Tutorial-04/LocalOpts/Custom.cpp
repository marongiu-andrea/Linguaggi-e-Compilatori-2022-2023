#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;


llvm::PreservedAnalyses run([[maybe_unused]] llvm::Function &F, llvm::FunctionAnalysisManager &AM) {
  auto &LI = AM.getResult<LoopAnalysis>(F);
}