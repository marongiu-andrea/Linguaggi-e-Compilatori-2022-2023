#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

PreservedAnalyses MultiInstructionOperationsPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "MultiInstructionOperations\n";
  return PreservedAnalyses::none();
}