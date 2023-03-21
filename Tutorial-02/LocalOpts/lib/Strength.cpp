#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
 
using namespace llvm;

static bool assignConstant(ConstantInt* &constant, Value*& operand, Instruction* Inst) {
  ConstantInt* firstOperand = dyn_cast<ConstantInt>(Inst->getOperand(0));
  ConstantInt* secondOperand = dyn_cast<ConstantInt>(Inst->getOperand(1));
  if(!firstOperand && !secondOperand)
    return false;
  else if(firstOperand) {
    constant = firstOperand;
    operand = Inst->getOperand(1);
  } else {
    constant = secondOperand;
    operand = Inst->getOperand(0);
  }
  return true;
}

static void insertInstruction(llvm::Instruction::BinaryOps type, Value* operand, int power, Instruction &Inst, int rest = 0) {
  Instruction *NewInst = BinaryOperator::Create(
    type,
    operand,
    ConstantInt::get(Inst.getContext() , APInt(32,  power))
  );  
  NewInst->insertAfter(&Inst);
  Inst.replaceAllUsesWith(NewInst);
  if(rest == -1) {
    Instruction *Sub = BinaryOperator::Create(
      Instruction::Sub,
      NewInst,
      operand
    );  
    Sub->insertAfter(NewInst);
  } else if(rest == 1) {
    Instruction *Add = BinaryOperator::Create(
      Instruction::Add,
      NewInst,
      operand
    );  
    Add->insertAfter(NewInst);
  }
}

bool strenghtReduction(BasicBlock &B) {
  ConstantInt *constant;
  Value *operand;
  for(BasicBlock::iterator I = B.begin(); I != B.end(); ++I) {
    Instruction &Inst = *I;
    if(Inst.getOpcode() == Instruction::Mul) {
      if(!assignConstant(constant, operand, &Inst))
        continue;
      int power = constant->getValue().nearestLogBase2();
      int rest = constant->getValue().roundToDouble(true) - pow(2, power);
      if(abs(rest) <= 1)
        insertInstruction(Instruction::Shl, operand, power, Inst, rest);

    } else if(Inst.getOpcode() == Instruction::SDiv) {
      constant = dyn_cast<ConstantInt>(Inst.getOperand(1));
      if(constant && constant->getValue().exactLogBase2() > 0) {
        insertInstruction(Instruction::AShr, Inst.getOperand(0),constant->getValue().exactLogBase2(), Inst);
      }
    }
  }
  return true;
}
  
static bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (strenghtReduction(*Iter)) {
      Transformed = true;
    } 
  }
  return Transformed;
}


PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

