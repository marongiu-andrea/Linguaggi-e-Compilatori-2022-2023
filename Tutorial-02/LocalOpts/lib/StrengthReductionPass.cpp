#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;


static void runOnBasicBlock(BasicBlock& bb) {
}

PreservedAnalyses StrengthReductionPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "Strength Reduction\n";

  for (auto bb = F.begin(); bb != F.end(); ++bb)
    runOnBasicBlock(*bb);

  return PreservedAnalyses::none();
}