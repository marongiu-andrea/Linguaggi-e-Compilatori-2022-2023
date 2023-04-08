#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockSRP(BasicBlock &B) {
    for(auto &Inst : B){
        if(Inst.getOpcode() == Instruction::Mul){

        }
    }
}

bool runOnFunctionSRP(Function &F) {
    bool Transformed = false;

    for (auto &Iter : F) {
      if (runOnBasicBlockSRP(Iter)) {
        Transformed = true;
      }
    }
  return Transformed;
};

PreservedAnalyses  StrengthReductionPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager   &) {
    for (auto &Iter : M) {
      if (runOnFunctionSRP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

  return PreservedAnalyses::all();
}
