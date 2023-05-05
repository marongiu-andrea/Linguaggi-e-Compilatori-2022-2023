#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;

PreservedAnalyses MyLoopFusionPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  outs() << "BEGINNING MyLoopFusionPass...\n";

  return PreservedAnalyses::none();
}