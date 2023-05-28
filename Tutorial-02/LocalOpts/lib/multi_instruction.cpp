#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include <vector>

using namespace llvm;

static std::vector<Instruction*> toRemove;

static bool multiInstructionOpt(BasicBlock& BB)
{
  for(Instruction& instruction : BB)
  {
    outs() << instruction << "\n";
    if(instruction.getNumOperands() != 2) continue;
    
    Value* op1 = instruction.getOperand(0);
    Value* op2 = instruction.getOperand(1);
    const ConstantInt* constVal2 = dyn_cast<ConstantInt>(op2);
    
    const uint32_t opcode = instruction.getOpcode();
    switch (opcode)
    {
      case Instruction::Add: {
        // case <var> + <const>
        if(constVal2)
        {
          BinaryOperator* binaryOp = dyn_cast<BinaryOperator>(op1);
          if (binaryOp && binaryOp->getOpcode() == Instruction::Sub)
          {
            if (binaryOp->getOperand(1) == op2)
            {
              outs() << "\nInstruction to remove:" << instruction 
                << "e rimpiazzo con " << *(binaryOp->getOperand(0)) << " \n";
              
              instruction.replaceAllUsesWith(binaryOp->getOperand(0));
              toRemove.push_back(&instruction);
            }
          }
        }
        
        break;
      }

      case Instruction::Sub: {
        // case <var> - <const>
        if(constVal2)
        {
          BinaryOperator* binaryOp = dyn_cast<BinaryOperator>(op1);
          if (binaryOp && binaryOp->getOpcode() == Instruction::Add)
          {
            if (binaryOp->getOperand(1) == op2)
            {
              outs() << "\nInstruction to remove:" << instruction 
                << "e rimpiazzo con " << *(binaryOp->getOperand(0)) << " \n";
              
              instruction.replaceAllUsesWith(binaryOp->getOperand(0));
              toRemove.push_back(&instruction);
            }
          }
        }
        
        break;
      }
    }

    outs() << "\n ---------------- \n";
  }

  for (Instruction* instruction : toRemove)
    instruction->eraseFromParent();

  return true;
}


static bool runOnFunction(Function& F) 
{
  bool transformed = false;

  for (auto it = F.begin(); it != F.end(); ++it)
  {
    if (multiInstructionOpt(*it)) 
    {
      transformed = true;
    } 
  }
  return transformed;
}

PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) 
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