#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;


bool algebricIdentity(BasicBlock& B) 
{
  int operands;

  for(BasicBlock::iterator itBB = B.begin(); itBB != B.end(); ++itBB) 
  {
    Instruction& instruction = *itBB;
    operands = 0;

    const uint32_t opcode = instruction.getOpcode();
    
    switch(opcode)
    {
      case Instruction::Mul:  {

        for (auto itInstr = instruction.op_begin(); itInstr != instruction.op_end(); ++itInstr) 
        {
          const Value* op = *itInstr;
          const ConstantInt* num = dyn_cast<ConstantInt>(op);

          if (num && num->getValue() == 1) 
          {
            instruction.replaceAllUsesWith(instruction.getOperand(1 - operands));
            --itBB;
            instruction.eraseFromParent();
          } 
          else 
            operands++;
        }
        break;
      }
      
      case Instruction::Add:  {

        for (auto itInstr = instruction.op_begin(); itInstr != instruction.op_end(); ++itInstr) 
        {
          const Value* op = *itInstr;
          const ConstantInt* num = dyn_cast<ConstantInt>(op);

          if (num && num->getValue() == 0) 
          {
            instruction.replaceAllUsesWith(instruction.getOperand(1 - operands));
            --itBB;
            instruction.eraseFromParent();
          } 
          else 
            operands++;
        }
      
        break;
      }

      case Instruction::Sub:  {
        const Value* op = instruction.getOperand(1); 
        const ConstantInt* num = dyn_cast<ConstantInt>(op);
        
        if (num && num->getValue() == 0) 
        {
          instruction.replaceAllUsesWith(instruction.getOperand(0));
          --itBB;
          instruction.eraseFromParent();
        }

        break;
      }

      case Instruction::SDiv: {
        const Value* op = instruction.getOperand(1); 
        const ConstantInt* num = dyn_cast<ConstantInt>(op);
        if (num && num->getValue() == 1)
        {
          instruction.replaceAllUsesWith(instruction.getOperand(0));
          --itBB;
          instruction.eraseFromParent();
        }

        break;
      }
    } 
  }
  return true;
}

static bool runOnFunction(Function& F) 
{
  bool transformed = false;

  for (auto it = F.begin(); it != F.end(); ++it)
  {
    if (algebricIdentity(*it)) 
    {
      transformed = true;
    } 
  }
  return transformed;
}

PreservedAnalyses AlgebricIdentityPass::run([[maybe_unused]] Module& M,ModuleAnalysisManager&) 
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