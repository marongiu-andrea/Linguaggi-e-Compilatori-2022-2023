#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "Strength Reduction\n";
  return PreservedAnalyses::none();
}