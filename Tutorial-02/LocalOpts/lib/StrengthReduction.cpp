#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runSROnBasicBlock(BasicBlock &B)
{

  for (auto Iter = ++B.begin(); Iter != B.end(); ++Iter)
  {
    Instruction &Inst = *Iter;
    if (Inst.isBinaryOp())
    {
      switch (Inst.getOpcode())
      {

      case Instruction::Mul:

        if (ConstantInt *C = dyn_cast<ConstantInt>(Inst.getOperand(0)))
          if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(1)))
          {
            int shift = C->getValue().nearestLogBase2();
            int resto = (int)C->getValue().roundToDouble() - (1 << shift);
            if (resto >= -1 && resto <= 1)
            {
              Type *Ty = C->getType();
              Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, I, ConstantInt::get(Ty, shift, true));
              NewInst->insertAfter(&Inst);
              if (resto==0)
                Inst.replaceAllUsesWith(NewInst);
              if (resto == 1)
              {
                Type *Ty = C->getType();
                //C = ConstantInt::get(Ty, 1, true);
                Instruction *NewInst2 = BinaryOperator::Create(Instruction::Add, NewInst, ConstantInt::get(Ty, 1, true));
                NewInst2->insertAfter(NewInst);
                Inst.replaceAllUsesWith(NewInst2);
              }
              if (resto == -1)
              {
                Type *Ty = C->getType();
                //C = ConstantInt::get(Ty, 1, true);
                Instruction *NewInst2 = BinaryOperator::Create(Instruction::Sub, NewInst, ConstantInt::get(Ty, 1, true));
                NewInst2->insertAfter(NewInst);
                Inst.replaceAllUsesWith(NewInst2);
              }
            }
          }
        if (ConstantInt *C = dyn_cast<ConstantInt>(Inst.getOperand(1)))
          if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(0)))
          {
            int shift = C->getValue().nearestLogBase2();
            int resto = (int)C->getValue().roundToDouble() - (1 << shift);
            if (resto >= -1 && resto <= 1)
            {
              Type *Ty = C->getType();
              Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, I, ConstantInt::get(Ty, shift, true));
              NewInst->insertAfter(&Inst);
              if (resto==0)
                Inst.replaceAllUsesWith(NewInst);
              if (resto == 1)
              {
                Type *Ty = C->getType();
                //C = ConstantInt::get(Ty, 1, true);
                Instruction *NewInst2 = BinaryOperator::Create(Instruction::Add, NewInst, ConstantInt::get(Ty, 1, true));
                NewInst2->insertAfter(NewInst);
                Inst.replaceAllUsesWith(NewInst2);
              }
              if (resto == -1)
              {
                Type *Ty = C->getType();
                //C = ConstantInt::get(Ty, 1, true);
                Instruction *NewInst2 = BinaryOperator::Create(Instruction::Sub, NewInst, ConstantInt::get(Ty, 1, true));
                NewInst2->insertAfter(NewInst);
                Inst.replaceAllUsesWith(NewInst2);
              }
            }
          }

        break;

      case Instruction::SDiv:

        if (ConstantInt *C = dyn_cast<ConstantInt>(Inst.getOperand(1)))
          if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(0)))
          {
            int shift = C->getValue().exactLogBase2();
            if (shift > 0)
            {
              Type *Ty = C->getType();
              Instruction *NewInst = BinaryOperator::Create(Instruction::AShr, I, ConstantInt::get(Ty, shift, true));
              NewInst->insertAfter(&Inst);
              Inst.replaceAllUsesWith(NewInst);
            }
          }
        break;
      }
    }
  }

  return true;
}

bool runSROnFunction(Function &F)
{
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
  {
    if (runSROnBasicBlock(*Iter))
    {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &)
{

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
  {
    if (runSROnFunction(*Iter))
    {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}
