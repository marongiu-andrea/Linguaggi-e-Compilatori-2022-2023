#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlock(BasicBlock &bb)
{
  std::list<Instruction*> toBeRemoved;

  // Algebraic Identity
  for(auto &inst: bb)
  {
    unsigned int opcode = inst.getOpcode();

    if(inst.isBinaryOp())
    {
      ConstantInt* opLeft = dyn_cast<ConstantInt>(inst.getOperand(0));
      ConstantInt* opRight = dyn_cast<ConstantInt>(inst.getOperand(1));
      APInt val;
      Value* kept = nullptr;

      if(opLeft)
      {
        val = opLeft->getValue();
        kept = inst.getOperand(1);
      }
      if(opRight)
      {
        val = opRight->getValue();
        kept = inst.getOperand(0);
      }

      if((val.isZero() && (opcode == Instruction::Add || opcode == Instruction::Sub))
        || (val.isOne() && (opcode == Instruction::Mul || opcode == Instruction::SDiv || opcode == Instruction::UDiv))
      )
      {
        outs() << inst << "\n";
        inst.replaceAllUsesWith(kept);
        toBeRemoved.push_back(&inst);
      }
    }
  }

  for(auto i: toBeRemoved)
  {
    i->eraseFromParent();
  }

  return true;
}


bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}




PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

