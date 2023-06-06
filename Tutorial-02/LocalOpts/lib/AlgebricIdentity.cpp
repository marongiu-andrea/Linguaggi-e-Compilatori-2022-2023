#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
 
using namespace llvm;


bool algIdentity(BasicBlock &B) {
  int operands;
  for(BasicBlock::iterator I = B.begin(); I != B.end(); ++I) {
    Instruction &Inst = *I;
    operands = 0;
    switch(Inst.getOpcode()) {
      case Instruction::Mul: {
        for (auto *Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter) {
          Value *Operand = *Iter;
          ConstantInt *C = dyn_cast<ConstantInt>(Operand);
          if (C && C->getValue() == 1) {
            Inst.replaceAllUsesWith(Inst.getOperand(1-operands));
            --I;
            Inst.eraseFromParent();
          } else {
            operands++;
          }
        }
        break;
      }
      case Instruction::Add: {
        for (auto *Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter) {
          Value *Operand = *Iter;
          ConstantInt *C = dyn_cast<ConstantInt>(Operand);
          if (C && C->getValue() == 0) {
            Inst.replaceAllUsesWith(Inst.getOperand(1-operands));
            --I;
            Inst.eraseFromParent();
          } else {
            operands++;
          }
        }
        break;
      }
      case Instruction::Sub: {
        
        Value *Operand = Inst.getOperand(1); 
        ConstantInt *C = dyn_cast<ConstantInt>(Operand);
        if (C && C->getValue() == 0) {
          Inst.replaceAllUsesWith(Inst.getOperand(0));
          --I;
          Inst.eraseFromParent();
        }
      
        break;
      }
      case Instruction::SDiv: {
        
        Value *Operand = Inst.getOperand(1); 
        ConstantInt *C = dyn_cast<ConstantInt>(Operand);
        if (C && C->getValue() == 1) {
          Inst.replaceAllUsesWith(Inst.getOperand(0));
          --I;
          Inst.eraseFromParent();
        }
      
        break;
      }
    }
  }
  return true;
}
  
static bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (algIdentity(*Iter)) {
      Transformed = true;
    } 
  }
  return Transformed;
}


PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}