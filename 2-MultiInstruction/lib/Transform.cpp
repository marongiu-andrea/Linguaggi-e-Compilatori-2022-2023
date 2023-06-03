#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>
using namespace llvm;

bool runOnBasicBlock(BasicBlock &B)
{
  Function *F = B.getParent();
  LLVMContext &context = F->getContext();

  int index = 1;
  Instruction *old_instr = nullptr;

  for (auto Iter = B.begin(); Iter != B.end(); ++Iter)
  {
    // Per ogni BasicBlock

    Instruction &Inst = *Iter;

    if (!strcmp(Inst.getOpcodeName(), "mul"))
    {
      // Se l'istruzione è una moltiplicazione

      int op_index = 0;

      for (auto *Iter = Inst.op_begin(); Iter != Inst.op_end(); ++Iter)
      {
        // Per ogni operando dell'istruzione

        Value *iter_op = *Iter;
        ConstantInt *C = dyn_cast<ConstantInt>(iter_op);

        if (C)
        {
          outs() << "Istruzione numero: " << index++ << " è una moltiplicazione \n";
          outs() << "\tIstruzione: " << Inst << "\n";
          outs() << "\t " << *C << ": sono una costante intera di valore " << C->getValue() << "\n";
          outs() << "\t trovato in pos: " << op_index << "\n";

          APInt intVal = C->getValue();
          float floatVal = static_cast<float>(intVal.getLimitedValue());
          float log_value = log2(floatVal);

          // se il log_value è pari alla sua floor vuol dire che value_In_float è effettivamente una potenza di 2
          // se non lo fosse  log_value sarebbe un valore con la virgola e quindi non valido
          if (log_value == floor(log_value))
          {
            int log_value_int = static_cast<int>(log_value);

            Value *operand_instr = nullptr;
            if (op_index == 0)
              operand_instr = Inst.getOperand(1);
            else
              operand_instr = Inst.getOperand(0);

            outs() << "\toperatore scelto " << *operand_instr << "\n";

            ConstantInt *log2ConstInt = ConstantInt::get(context, APInt(32, 1));
            Instruction *NewInst = BinaryOperator::CreateShl(operand_instr, log2ConstInt, "NewInst");
            outs() << "\tNuova istruzione" << *NewInst << "\n";
            NewInst->insertAfter(&Inst);

            Inst.replaceAllUsesWith(NewInst);
          }
        }
        op_index++;
      }
    }
    old_instr = &(*Iter);
  }
  return true;
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

