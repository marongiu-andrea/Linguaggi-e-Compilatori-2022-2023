#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

bool algebric_transformation(BasicBlock &B){

}


PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  return PreservedAnalyses::none();
}