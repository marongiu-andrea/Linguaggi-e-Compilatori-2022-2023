#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;


bool algebricIdentity(BasicBlock& B) 
{
  // itero sulle istruzioni del basic block
  for(BasicBlock::iterator itBB = B.begin(); itBB != B.end(); ++itBB) 
  {
    Instruction& instruction = *itBB;
    outs() << instruction << "\n";

    if(instruction.getNumOperands() <= 1) continue;

    Value* op1 = instruction.getOperand(0);
    Value* op2 = instruction.getOperand(1);
    outs() << "\toperand 1=" << op1 << "\n";
    outs() << "\toperand 2=" << op2 << "\n";

    const uint32_t opcode = instruction.getOpcode();
    switch(opcode)
    {
      case Instruction::Mul:  {
        const ConstantInt* valNum = dyn_cast<ConstantInt>(op1);
        
        // case 1 x var
        if (valNum && valNum->getValue() == 1) 
          instruction.replaceAllUsesWith(op2);
        // case var x 1
        else
          instruction.replaceAllUsesWith(op1);

        --itBB;
        instruction.eraseFromParent();

        break;
      }
      
      case Instruction::Add:  {
        const ConstantInt* valNum = dyn_cast<ConstantInt>(op1);
        
        // case 0 + var
        if (valNum && valNum->getValue() == 0) 
          instruction.replaceAllUsesWith(op2);
        // case var + 0
        else
          instruction.replaceAllUsesWith(op1);

        --itBB;
        instruction.eraseFromParent();

        break;
      }
    } 

    outs() << "\n ---------------- \n";
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