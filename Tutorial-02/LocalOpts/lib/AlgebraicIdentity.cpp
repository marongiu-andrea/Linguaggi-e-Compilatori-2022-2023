#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runAIOnBasicBlock(BasicBlock &B) {

    for (auto Iter = B.begin(); Iter != B.end(); ++Iter){
      Instruction &Inst = *Iter;
      if (Inst.isBinaryOp())
        switch (Inst.getOpcode()){
          case Instruction::Add:
              if (ConstantInt *CI = dyn_cast<ConstantInt>(Inst.getOperand(0)))
                if(CI->isZero())
                  //for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter) {
                    Inst.replaceAllUsesWith(Inst.getOperand(1));
                 // }
          case Instruction::Sub:
              if (ConstantInt *CI = dyn_cast<ConstantInt>(Inst.getOperand(1)))
                if (CI->isZero())
                  //for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter)
                      Inst.replaceAllUsesWith(Inst.getOperand(0));
              break;
          case Instruction::Mul:
              if (ConstantInt *CI = dyn_cast<ConstantInt>(Inst.getOperand(0)))
                if (CI->isOne())
                 // for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter)
                      Inst.replaceAllUsesWith(Inst.getOperand(1));
          case Instruction::UDiv:
          case Instruction::SDiv:
              if (ConstantInt *CI = dyn_cast<ConstantInt>(Inst.getOperand(1)))
                if (CI->isOne())
                  //for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter)
                      Inst.replaceAllUsesWith(Inst.getOperand(0));
              break;
        }

    }
    
    return true;
  }


  bool runAIOnFunction(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runAIOnBasicBlock(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runAIOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

