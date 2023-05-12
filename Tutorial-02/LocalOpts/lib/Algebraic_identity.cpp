#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockAIP(BasicBlock &B) {
  for(auto &Inst : B){
    if(Inst.getOpcode() == Instruction::Add || Inst.getOpcode() == Instruction::Sub){

      ConstantInt *op2 = dyn_cast<ConstantInt>(Inst.getOperand(1));

      outs()<< "Inutile in add/sub: "<< Inst <<"\n";

      if(op2 && op2->getValue().isZero())
        Inst.replaceAllUsesWith(Inst.getOperand(0));
    }
    if(Inst.getOpcode() == Instruction::Mul || Inst.getOpcode() == Instruction::SDiv){

      ConstantInt *op1 = dyn_cast<ConstantInt>(Inst.getOperand(0));
      ConstantInt *op2 = dyn_cast<ConstantInt>(Inst.getOperand(1));

      outs()<< "Inutile in mul/div: "<< Inst <<"\n";

      if(op2 && op2->getValue().isOneValue())
        Inst.replaceAllUsesWith(Inst.getOperand(0));
      if(op1 && op1->getValue().isOneValue())
        Inst.replaceAllUsesWith(Inst.getOperand(1));
    }
  }
};

bool runOnFunctionAIP(Function &F) {
    bool Transformed = false;

    for (auto &Iter : F) {
      if (runOnBasicBlockAIP(Iter)) {
        Transformed = true;
      }
    }
  return Transformed;
};

PreservedAnalyses  AlgebraicIdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager   &) {
    for (auto &Iter : M) {
      if (runOnFunctionAIP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

  return PreservedAnalyses::all();
}
