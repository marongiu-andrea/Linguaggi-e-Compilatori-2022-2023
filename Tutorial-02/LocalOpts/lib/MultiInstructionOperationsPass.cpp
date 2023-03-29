#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

PreservedAnalyses MultiInstructionOperationsPass::run(Function &F,
                                                      FunctionAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "MultiInstructionOperations\n";
  return PreservedAnalyses::none();
}