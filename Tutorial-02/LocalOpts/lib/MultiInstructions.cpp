#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <map>
 
using namespace llvm;

std::map<unsigned, unsigned> instructionMap = {
  {Instruction::Add, Instruction::Sub},
  {Instruction::Sub, Instruction::Add},
};

bool instructionCheck(ConstantInt* first, ConstantInt* second, unsigned type, Instruction* Inst) {
  if(Inst->getOpcode() == type && first->getValue() == second->getValue()) {
    return true;
  }
  return false;
}

bool assignConstant(ConstantInt* &constant, Value*& operand, Instruction* Inst) {
  ConstantInt* firstOperand = dyn_cast<ConstantInt>(Inst->getOperand(0));
  ConstantInt* secondOperand = dyn_cast<ConstantInt>(Inst->getOperand(1));
  if(firstOperand && secondOperand || !firstOperand && !secondOperand)
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

bool multiInstructionOptimization(BasicBlock &B) {
  ConstantInt *firstConstant, *secondConstant;
  Value* firstInstOperand, *secondInstOperand;
  for(BasicBlock::iterator I = B.begin(); I != B.end(); ++I) {
    Instruction &Inst = *I;
    if(Inst.isBinaryOp() && instructionMap[Inst.getOpcode()]) {
      if(!assignConstant(firstConstant, firstInstOperand, &Inst))
        continue;
      for (auto Iter = Inst.user_begin(); Iter != Inst.user_end(); ++Iter) {
        Instruction *User = dyn_cast<Instruction>(*Iter);
        if(User->isBinaryOp() && instructionMap[Inst.getOpcode()]) {
          if(!assignConstant(secondConstant, secondInstOperand, User))
            continue;
          if(instructionCheck(firstConstant, secondConstant, instructionMap[Inst.getOpcode()], User)) {
            User->replaceAllUsesWith(firstInstOperand);
          }
        }
      }
    }
  }
  return true;
}
  
static bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (multiInstructionOptimization(*Iter)) {
      Transformed = true;
    } 
  }
  return Transformed;
}


PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

