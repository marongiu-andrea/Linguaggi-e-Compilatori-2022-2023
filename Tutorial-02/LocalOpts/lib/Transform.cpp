#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <math.h>
#include <list>

using namespace llvm;

bool isPowOfTwo(int n);

bool runOnBasicBlock(BasicBlock &bb)
{
  std::list<Instruction *> toBeRemoved;

  for (auto &inst : bb)
  {
    if (inst.getOpcode() == Instruction::Mul)
    {
      outs() << inst << "\n";

      ConstantInt *opLeft  = dyn_cast<ConstantInt>(inst.getOperand(0));
      ConstantInt *opRight = dyn_cast<ConstantInt>(inst.getOperand(1));

      Instruction *newInst = nullptr;

      if (opLeft)
      {
        APInt val = opLeft->getValue();
        outs() << "OpLeft: " << val << "\n";
        auto opType = opLeft->getType();
        
        auto newOperand = ConstantInt::get(opType, val.exactLogBase2());

        if (newOperand->getValue().isNonNegative())
        {
          outs() << val << " è potenza del 2\n\n";
          outs() << "shift di: " << newOperand->getZExtValue() << "\n\n";

          newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(1), newOperand);
        }
      }
      else if (opRight)
      {
        APInt val = opRight->getValue();
        outs() << "opRight: " << val << "\n";
        auto opType = opRight->getType();
        
        auto newOperand = ConstantInt::get(opType, val.exactLogBase2());

        if (newOperand->getValue().isNonNegative())
        {
          outs() << val << " è potenza del 2\n\n";
          outs() << "shift di: " << newOperand->getZExtValue() << "\n\n";

          newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(0), newOperand);
        }
      }
      else
        outs() << "Nessun operando costante\n\n";

      if (newInst)
      {
        newInst->insertBefore(&inst);
        inst.replaceAllUsesWith(newInst);
        toBeRemoved.push_back(&inst);
      }
    }
  }

  for (auto i : toBeRemoved)
    i->eraseFromParent();

  return true;
}

bool isPowOfTwo(int n)
{
  if (n <= 1)
    return false;

  return floor(log2(n)) == ceil(log2(n));
}

bool runOnFunction(Function &F)
{
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
  {
    if (runOnBasicBlock(*Iter))
    {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,
                                     ModuleAnalysisManager &)
{

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
  {
    if (runOnFunction(*Iter))
    {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}
