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

            // Si possono aggiornare le singole references separatamente?
            // Controlla la documentazione e prova a rispondere.
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

/*
  CODICE VECCHIO

bool runOnBasicBlock(BasicBlock &B)
{
  Function *F = B.getParent();
  LLVMContext &context = F->getContext();

  // Preleviamo le prime due istruzioni del BB
  Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

  // L'indirizzo della prima istruzione deve essere uguale a quello del
  // primo operando della seconda istruzione (per costruzione dell'esempio)
  assert(&Inst1st == Inst2nd.getOperand(0));

  // Stampa la prima istruzione
  outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
  outs() << "SECONDA ISTRUZIONE: " << Inst2nd << "\n";
  // Stampa la prima istruzione come operando
  outs() << "COME OPERANDO: ";
  Inst1st.printAsOperand(outs(), false);
  outs() << "\n";

  // User-->Use-->Value
  outs() << "I MIEI OPERANDI SONO:\n";
  for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter)
  {
    Value *Operand = *Iter;

    if (Argument *Arg = dyn_cast<Argument>(Operand))
    {
      outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo()
             << " DELLA FUNZIONE" << Arg->getParent()->getName()
             << "\n";
    }
    if (ConstantInt *C = dyn_cast<ConstantInt>(Operand))
    {
      outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue()
             << "\n";
    }
  }

  outs() << "LA LISTA DEI MIEI USERS:\n";
  for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter)
  {
    outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
  }

  outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
  for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter)
  {
    outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
  }

  // Manipolazione delle istruzioni
  Instruction *NewInst = BinaryOperator::Create(
      Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

  NewInst->insertAfter(&Inst1st);
  // Si possono aggiornare le singole references separatamente?
  // Controlla la documentazione e prova a rispondere.
  Inst1st.replaceAllUsesWith(NewInst);

  for (auto *Iter_op = Inst.op_begin(); Iter_op != Inst.op_end(); ++Iter_op)
  {
    Value *Operand = *Iter_op;

    if (ConstantInt *C = dyn_cast<ConstantInt>(Operand))
    {
      outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue() << "\n";

      /*
      Questo serve perchè LLVM restituisce un intero forzato quando si usa getValue()
      per aggirare la cosa si usa una libreria che estrae l'intero e poi fa il cast a float
      questo serve perchè la funzione log2 prende un float come parametro.

      uint64_t value = C->getValue().getZExtValue();  // extract the integer value from the APInt object
      float value_in_float = static_cast<float>(value);
      float log_value = log2(value_in_float);
      //se il log_value è pari alla sua floor vuol dire che value_In_float è effettivamente una potenza di 2
      //se non lo fosse  log_value sarebbe un valore con la virgola e quindi non valido
      if(log_value == floor(log_value)){
        outs() << "\tPossiamo fare un miglioramento " << "\n";
        //outs() << "\t Primo operando: " << Inst.getOperand(0) << "\n";
        //int value_of_log_in_int = APInt IntValue(32, log_value);
        /*
        LLVMContext context;
        Value *log_value_in_constant_int = ConstantInt::get(Type::getInt32Ty(context),log_value);
        Instruction *NewInst = BinaryOperator::CreateShl(
        Inst.getOperand(0), log_value_in_constant_int);

        NewInst->insertAfter(old_instr);
        // Si possono aggiornare le singole references separatamente?
        // Controlla la documentazione e prova a rispondere.
        Inst.replaceAllUsesWith(NewInst);

}

for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter)
{
  Value *Operand = *Iter;
}
old_instr = &(*Iter);
}
return true;
}
*/