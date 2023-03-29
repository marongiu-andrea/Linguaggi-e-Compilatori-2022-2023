#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <string.h>
using namespace llvm;

bool runOnBasicBlock2(BasicBlock &B) {

  for(auto Iter=B.begin(); Iter != B.end(); ++Iter){
      Instruction &Inst = *Iter;
      if(Inst.getOpcode()==Instruction::Add){
          ConstantInt *CI=dyn_cast<ConstantInt>(Inst.getOperand(1));
           outs()<<"ADDIZIONE | ";
           if(CI && (CI->getValue()).isZero()){
                outs()<<"ADDIZIONE NULLA\n";
                Inst.replaceAllUsesWith(Inst.getOperand(0));
            } 
        }
      if(Inst.getOpcode()==Instruction::Mul){
        ConstantInt *c1 = dyn_cast<ConstantInt>(Inst.getOperand(0));
        ConstantInt *c2 = dyn_cast<ConstantInt>(Inst.getOperand(1));
        if (c1 && c1->getValue().isOneValue())
          Inst.replaceAllUsesWith(Inst.getOperand(1));    
        else if (c2 && c2->getValue().isOneValue())
          Inst.replaceAllUsesWith(Inst.getOperand(0));   
      }
  }

    return true;
  }


  bool runOnFunction2(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlock2(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction2(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

