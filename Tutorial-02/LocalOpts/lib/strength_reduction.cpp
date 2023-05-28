#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>
#include <list>

using namespace llvm;

static void insertInstruction(
  Instruction::BinaryOps binaryOps, 
  Value* operand, 
  Instruction& instruction,
  const double power,
  const double rest
)
{
  Instruction* newInstr = BinaryOperator::Create(
    binaryOps,
    operand,
    ConstantInt::get(instruction.getContext() , APInt(32,  power))
  ); 
  newInstr->insertAfter(&instruction);
  instruction.replaceAllUsesWith(newInstr);

  if(rest == 1) 
  {
    Instruction* addInstr = BinaryOperator::Create(Instruction::Add, newInstr, operand);  
    addInstr->insertAfter(newInstr);
  }
  else
  {
    Instruction* subInstr = BinaryOperator::Create(Instruction::Sub, newInstr, operand);  
    subInstr->insertAfter(newInstr);
  }

}

bool strenghtReduction(BasicBlock& BB) 
{
  for (Instruction& instruction : BB)
  {
    outs() << instruction << "\n";

    if(instruction.getNumOperands() != 2) continue;

    Value* op1 = instruction.getOperand(0);
    Value* op2 = instruction.getOperand(1);
    outs() << "\toperand 1=" << op1 << "\n";
    outs() << "\toperand 2=" << op2 << "\n";

    ConstantInt* constOp1 = dyn_cast<ConstantInt>(op1);
    ConstantInt* constOp2 = dyn_cast<ConstantInt>(op2);
    // case <var> op <var>
    if(!constOp1 && !constOp2) continue;

    const uint32_t opcode = instruction.getOpcode();
    switch(opcode)
    {
      case Instruction::Mul: {
        double power;
        double rest;

        // case <var> x <const>
        if(constOp2)
        {
          power = (double)constOp2->getValue().nearestLogBase2();
          rest  = (double)constOp2->getValue().roundToDouble(true) - pow(2, power);
          if(abs(rest) <= 1)
            insertInstruction(Instruction::Shl, op2, instruction, power, rest);
        }
        // case <const> x <var> 
        else
        {
          power = (double)constOp1->getValue().nearestLogBase2();
          rest  = (double)constOp1->getValue().roundToDouble(true) - pow(2, (double)power);
          if(abs(rest) <= 1)
            insertInstruction(Instruction::Shl, op1, instruction, power, rest);
        }
        break;
      }

      case Instruction::SDiv: {
        // case <var> / <const>
        if(constOp2)
        {
          const APInt& value = constOp2->getValue();
          if(value.exactLogBase2() > 0)
            insertInstruction(Instruction::AShr, op1, instruction, value.exactLogBase2(), 0);
        }
        break;
      }
    }

    outs() << "\n ---------------- \n";
  }

  return true;
}

static bool runOnFunction(Function &F) 
{
  bool transformed = false;

  for (auto it = F.begin(); it != F.end(); ++it)
  {
    if (strenghtReduction(*it)) 
    {
      transformed = true;
    } 
  }
  return transformed;
}

PreservedAnalyses StrengthReductionPass::run(
  [[maybe_unused]] Module& M,ModuleAnalysisManager&) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto it = M.begin(); it != M.end(); ++it) 
  {
    if (runOnFunction(*it)) 
    {
      return PreservedAnalyses::none();
    }
  }
  return PreservedAnalyses::none();
}
