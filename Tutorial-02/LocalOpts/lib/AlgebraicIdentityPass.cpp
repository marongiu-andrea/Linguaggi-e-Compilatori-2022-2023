#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

PreservedAnalyses AlgebraicIdentityPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "AlgebraicIdentity\n";
  return PreservedAnalyses::none();
}