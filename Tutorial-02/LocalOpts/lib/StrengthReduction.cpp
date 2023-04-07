#include "LocalOpts.h"
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

  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    Instruction &Inst = *Iter;
    if (Inst.getOpcode() == Instruction::Mul) {
      ConstantInt *c1 = dyn_cast<ConstantInt>(Inst.getOperand(0));
      ConstantInt *c2 = dyn_cast<ConstantInt>(Inst.getOperand(1));
      if (c1) {
        if (isPowerOf2_32(c1->getZExtValue())) {
          if (!c1->isOneValue()) {
            outs() << "Potenza Rilevata c1 con logaritmo intero\n";
            Value *c2 = Inst.getOperand(1);
            Value *NewInst = BinaryOperator::CreateShl(
                c2, ConstantInt::get(c1->getType(), c1->getValue().logBase2()),
                "", &Inst);
            Inst.replaceAllUsesWith(NewInst);
            // Inst.removeFromParent();
          }
        } else {
          outs() << "Potenza Rilevata c1 con logaritmo floating point\n";
        }

      } else if (c2) {
        if (isPowerOf2_32(c2->getZExtValue())) {
          if (!c2->isOneValue()) {
            outs() << "Potenza Rilevata c2 con logaritmo intero\n";
            Value *c1 = Inst.getOperand(0);
            Value *NewInst = BinaryOperator::CreateShl(
                c1, ConstantInt::get(c2->getType(), c2->getValue().logBase2()),
                "", &Inst);
            Inst.replaceAllUsesWith(NewInst);
            // Inst.eraseFromParent();//Probabilnmente non da usare in
            // ottimizzazione finale
          }
        } else {
          outs() << "Potenza Rilevata c2 con logaritmo floating point\n"<<ConstantInt::get(c2->getType(), c2->getValue().ceilLogBase2())->getValue();
          Value *c1 = Inst.getOperand(0);
          Instruction *PartialMul = BinaryOperator::CreateShl(
              c1,
              ConstantInt::get(c2->getType(), c2->getValue().ceilLogBase2()),
              "",&Inst);
           //Inst.replaceAllUsesWith(PartialMul);
           Value *AddCarriage = BinaryOperator::CreateAdd(
            PartialMul,
            ConstantInt::get(c2->getType(),c2->getValue()-pow(2,c2->getValue().ceilLogBase2())),
            "",
            &Inst
            );
            Inst.replaceAllUsesWith(AddCarriage);
        }
      }
    }
    if (Inst.getOpcode() == Instruction::FDiv) {
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