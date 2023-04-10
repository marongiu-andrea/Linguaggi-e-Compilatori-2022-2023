#include "LocalOpts.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <cmath>
#include <cstdint>
#include <math.h>
#include <string.h>
using namespace llvm;

bool runOnBasicBlock3(BasicBlock &B) {
  Function *F = B.getParent();
  LLVMContext &context = F->getContext();
  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    Instruction &Inst = *Iter;

    if (Inst.getOpcode() == Instruction::Mul){
      ConstantInt* c1 = dyn_cast<ConstantInt>(Inst.getOperand(0));
      ConstantInt* c2 = dyn_cast<ConstantInt>(Inst.getOperand(1));
     
      if (c1)
        if (c1->getValue().isPowerOf2()){
          outs() <<"MUL: "<< c1->getValue() << " op1 è una pow di 2\n";
          outs() << "Faccio left shift di " << c1->getValue().logBase2() << "\n";
          Instruction* newInst = BinaryOperator::CreateShl(Inst.getOperand(1),ConstantInt::get(c1->getType(),c1->getValue().logBase2()));
          newInst->insertAfter(&Inst);
          Inst.replaceAllUsesWith(newInst);
        } else {
          outs() << "MUL: op1 " << c1->getValue() << " non è pow di 2\n";
          int nPow2 = pow(2, floor(c1->getValue().logBase2()) + 1);

          //shift di log2(nPow2)
          ConstantInt *numShift = ConstantInt::get(context, APInt(32,log2(nPow2)));
          Instruction *partialMul = BinaryOperator::CreateShl(Inst.getOperand(1),numShift);  
          
          partialMul->insertAfter(&Inst);   
          Instruction *newInst = BinaryOperator::CreateSub(partialMul,Inst.getOperand(1));
          newInst->insertAfter(partialMul);
          Inst.replaceAllUsesWith(newInst);  

        }
      else if (c2)
        if (c2->getValue().isPowerOf2()){
          outs() <<"MUL: "<< c2->getValue() << " op2 è una pow di 2\n";
          outs() << "Faccio left shift di " << c2->getValue().logBase2() << "\n";
          Instruction* newInst = BinaryOperator::CreateShl(Inst.getOperand(0),ConstantInt::get(c2->getType(),c2->getValue().logBase2()));
          newInst->insertAfter(&Inst);
          Inst.replaceAllUsesWith(newInst);
        } else { 
          outs() << "MUL: op2 " << c2->getValue() << " non è pow di 2\n";

          //calcolo pow 2 più vicina maggiore di op2
          int nPow2 = pow(2, floor(c2->getValue().logBase2()) + 1);

          //shift di log2(nPow2)
          ConstantInt *numShift = ConstantInt::get(context, APInt(32,log2(nPow2)));
          Instruction *partialMul = BinaryOperator::CreateShl(Inst.getOperand(0),numShift);     
          
          partialMul->insertAfter(&Inst);
          Instruction *newInst = BinaryOperator::CreateSub(partialMul,Inst.getOperand(0));
          newInst->insertAfter(partialMul);
          Inst.replaceAllUsesWith(newInst);   
        }  
    }
    if (Inst.getOpcode() == Instruction::SDiv){
      ConstantInt *c2 = dyn_cast<ConstantInt>(Inst.getOperand(1));
      if (c2 && !c2->getValue().isZero()){
        if (c2->getValue().isPowerOf2()){
          outs() << "Sono nella divisione\n op2= " << c2->getValue() << "\n";
          Instruction *newInst = BinaryOperator::CreateLShr(Inst.getOperand(0),ConstantInt::get(c2->getType(),c2->getValue().logBase2()));
          newInst->insertAfter(&Inst);
          Inst.replaceAllUsesWith(newInst);
        }
      }
    }
  
}

return true;
}

bool runOnFunction3(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock3(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction3(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}