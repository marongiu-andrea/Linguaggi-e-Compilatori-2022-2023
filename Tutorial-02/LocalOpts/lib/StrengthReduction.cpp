#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "math.h"

using namespace llvm;

bool runOnBB(BasicBlock &bb)
{
  std::list<Instruction*> toBeRemoved;

  // Strength Reduction
  for(auto &inst: bb)
  {
    unsigned int opcode = inst.getOpcode();

    if(inst.isBinaryOp())
    {
      ConstantInt* opLeft = dyn_cast<ConstantInt>(inst.getOperand(0));
      ConstantInt* opRight = dyn_cast<ConstantInt>(inst.getOperand(1));
      APInt val;

      Value* kept = nullptr;

      if(opLeft && !opRight)
      {
        val = opLeft->getValue();
        kept = inst.getOperand(1);
      }
      if(opRight && !opLeft)
      {
        val = opRight->getValue();
        kept = inst.getOperand(0);
      }

      if(kept && (opcode == Instruction::Mul || opcode == Instruction::SDiv || opcode == Instruction::UDiv))
      {
        if(!val.isZero())   // because of a bug, the parameter %0 is considered null at runtime, this is a temporary fix
        {
          int log2OfVal = val.exactLogBase2();

          outs() << inst << "\n";

          if(opcode == Instruction::Mul && log2OfVal < 0)   // val is not an exact power of two
          {
              outs() << "Applying advanced strength reduction\n";

              int nearestLog2 = val.nearestLogBase2();    

              APInt remainder = val- pow(2,nearestLog2);
              
              outs() << "nearest Log2: " << nearestLog2 << "\n";
              outs() << "remainder: " << remainder << "\n";

              if(remainder.abs().isOne())
              {
                  IntegerType* type = dyn_cast<IntegerType>(kept->getType());

                  if(type)
                  {
                      ConstantInt* newOperand = ConstantInt::get(type, nearestLog2);

                      Instruction* shiftInst = BinaryOperator::Create(Instruction::Shl, kept, newOperand);
                      outs() << "Addying shl op\n";

                      Instruction* remInst;

                      if(remainder.isOne())
                      {
                          remInst = BinaryOperator::Create(Instruction::Add, shiftInst, kept);
                          outs() << "addying add op\n\n";
                      }
                      else
                      {
                          remInst = BinaryOperator::Create(Instruction::Sub, shiftInst, kept);
                          outs() << "addying sub op\n\n";
                      }

                      if(shiftInst && remInst)
                      {
                          shiftInst->insertBefore(&inst);
                          remInst->insertAfter(shiftInst);

                          inst.replaceAllUsesWith(remInst);
                          toBeRemoved.push_back(&inst);
                      }
                  }
              }
          }
          else if(log2OfVal > 0)  // standard strength reduction
          {
            outs() << "Applying standard strength reduction\n\n";

            IntegerType* type = dyn_cast<IntegerType>(kept->getType());
            if(type)
            {
              ConstantInt* newOperand = ConstantInt::get(type, log2OfVal);

              Instruction* shiftInst;

              if(opcode == Instruction::Mul)
                shiftInst = BinaryOperator::Create(Instruction::Shl, kept, newOperand);
              else
                shiftInst = BinaryOperator::Create(Instruction::AShr, kept, newOperand);

              if(shiftInst)
              {
                shiftInst->insertBefore(&inst);
                inst.replaceAllUsesWith(shiftInst);
                toBeRemoved.push_back(&inst);
              }
            }
          }
          else
          {
            outs() << "Nothing done here\n\n";
          }
        }
      }
    }
  }

  for(auto i: toBeRemoved)
  {
    i->eraseFromParent();
  }

  return true;
}

bool runOnFunc(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBB(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}




PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunc(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

