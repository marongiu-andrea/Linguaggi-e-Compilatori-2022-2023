
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
        uint val = opLeft->getZExtValue();

        outs() << "OpLeft: " << val << "\n";
        if (val > 0 and isPowOfTwo(int(val)))
        {
          outs() << val << " è potenza del 2\n\n";

          Value *newOp = ConstantInt::get(IntegerType::get(bb.getContext(), 32), log2(val), false);

          outs() << "shift di: " << dyn_cast<ConstantInt>(newOp)->getZExtValue() << "\n\n";

          newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(1), newOp);
        }
      }
      else if (opRight)
      {
        uint val = opRight->getZExtValue();

        outs() << "OpRight: " << val << "\n";

        if (val > 0 and isPowOfTwo(val))
        {
          outs() << val << " è potenza del 2\n";

          Value *newOp = ConstantInt::get(IntegerType::get(bb.getContext(), 32), log2(val), false);

          outs() << "shift di: " << dyn_cast<ConstantInt>(newOp)->getZExtValue() << "\n\n";

          newInst = BinaryOperator::Create(Instruction::Shl, inst.getOperand(0), newOp);
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