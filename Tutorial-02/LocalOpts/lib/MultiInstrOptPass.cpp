#include "LocalOpts.h"
#include <llvm/IR/BasicBlock.h>
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

//Checks if the two operands of the input are instructions. If the first one is, if his instruction is a difference and his second operand is
//equal to the second operand of the input, then the function replaces the input uses with the first operand of the first operand of the input and returns true.
//The same is done with the second operand of the input but this time the function checks that the second operand of the second operand of the input is equal
//to the first operand of the input and the input is replaced with the first operand of the second operand of the input.
bool multi_op_add(Instruction &inst){
  Instruction *op0 = dyn_cast<Instruction>(inst.getOperand(0));
  Instruction *op1 = dyn_cast<Instruction>(inst.getOperand(1));
  if(op0 && op0->getOpcode() == Instruction::Sub && op0->getOperand(1) == inst.getOperand(1)){
      inst.replaceAllUsesWith(op0->getOperand(0));
      return true;
  }
  if(op1 && op1->getOpcode() == Instruction::Sub && op1->getOperand(1) == inst.getOperand(0)){
      inst.replaceAllUsesWith(op1->getOperand(0));
      return true;
  }
  return false;
}

bool multi_op_sub(Instruction &inst){
  Instruction *op0 = dyn_cast<Instruction>(inst.getOperand(0));
  Instruction *op1 = dyn_cast<Instruction>(inst.getOperand(1));
  if(op0 && op0->getOpcode() == Instruction::Add){
    if(op0->getOperand(0) == inst.getOperand(1)){
      inst.replaceAllUsesWith(op0->getOperand(1));
      return true;
    }
    if(op0->getOperand(1) == inst.getOperand(1)){
      inst.replaceAllUsesWith(op0->getOperand(0));
      return true;
    }
  }
  
  if(op1 && op1->getOpcode() == Instruction::Add){
    if(op1->getOperand(0) == inst.getOperand(0)){
      llvm::Type *i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
      Instruction *newinst = BinaryOperator::Create(Instruction::Sub, ConstantInt::get(i64_type,0), op1->getOperand(1));
      newinst->insertAfter(&inst);
      inst.replaceAllUsesWith(newinst);
      return true;
    }
    if(op1->getOperand(1) == inst.getOperand(0)){
      llvm::Type *i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
      Instruction *newinst = BinaryOperator::Create(Instruction::Sub, ConstantInt::get(i64_type,0), op1->getOperand(0));
      newinst->insertAfter(&inst);
      inst.replaceAllUsesWith(newinst);
      return true;      
    }
  }
  return false;
}

bool multi_op_mul(Instruction &inst){
  Instruction *op0 = dyn_cast<Instruction>(inst.getOperand(0));
  Instruction *op1 = dyn_cast<Instruction>(inst.getOperand(1));
  if(op0 && (op0->getOpcode() == Instruction::SDiv || op0->getOpcode() == Instruction::UDiv) && op0->getOperand(1) == inst.getOperand(1)){
    inst.replaceAllUsesWith(op0->getOperand(0));
    return true;
  }
  if(op1 && (op1->getOpcode() == Instruction::SDiv || op1->getOpcode() == Instruction::UDiv) && op1->getOperand(1) == inst.getOperand(0)){
    inst.replaceAllUsesWith(op1->getOperand(0));
    return true;
  }
  return false;
}

bool multi_op_div(Instruction &inst){
  Instruction *op0 = dyn_cast<Instruction>(inst.getOperand(0));
  Instruction *op1 = dyn_cast<Instruction>(inst.getOperand(1));
  if(op0 && op0->getOpcode() == Instruction::Mul){
    if(op0->getOperand(0) == inst.getOperand(1)){
      inst.replaceAllUsesWith(op0->getOperand(1));
      return true;
    }
    if(op0->getOperand(1) == inst.getOperand(1)){
      inst.replaceAllUsesWith(op0->getOperand(0));
      return true;
    }
  }
  if(op1 && op1->getOpcode() == Instruction::Mul){
    if(op1->getOperand(0) == inst.getOperand(0)){
      llvm::Type *i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
      Instruction *newinst = BinaryOperator::Create(Instruction::SDiv, ConstantInt::get(i64_type,1), op1->getOperand(1));
      newinst->insertAfter(&inst);
      inst.replaceAllUsesWith(newinst);
      return true;            
    }
    if(op1->getOperand(1) == inst.getOperand(0)){
      llvm::Type *i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
      Instruction *newinst = BinaryOperator::Create(Instruction::SDiv, ConstantInt::get(i64_type,1), op1->getOperand(0));
      newinst->insertAfter(&inst);
      inst.replaceAllUsesWith(newinst);
      return true;            
    }
  }
  return false;
}
bool MultiInstrOptPass::runOnBasicBlock(llvm::BasicBlock& bb)
{
    bool transformed = false;
    SmallVector<Instruction*, 0> to_delete;

    for (auto iter = bb.begin(); iter != bb.end(); ++iter){
      Instruction &inst = *iter;

      switch (inst.getOpcode()){
        case Instruction::Add:
          if(multi_op_add(inst)){
            to_delete.push_back(&inst);
            transformed = true;
          }
          break;
        
        case Instruction::Sub:          
          if(multi_op_sub(inst)){
            to_delete.push_back(&inst);
            transformed = true;
          }       
          break;

        case Instruction::Mul:
          if(multi_op_mul(inst)){
            to_delete.push_back(&inst);
            transformed = true;
          }        
          break;

        case Instruction::SDiv:
          if(multi_op_div(inst)){
            to_delete.push_back(&inst);
            transformed = true;
          }
          break;      

        case Instruction::UDiv:
          if(multi_op_add(inst)){
            to_delete.push_back(&inst);
            transformed = true;
          }
          break;        
      }

    }
    for (auto instr = to_delete.begin(); instr != to_delete.end(); ++instr){
      (*instr)->eraseFromParent();
    }

    return transformed;
}