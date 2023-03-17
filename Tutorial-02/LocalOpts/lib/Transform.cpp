#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include <string.h>
#include <cmath>
using namespace llvm;

bool runOnBasicBlock(BasicBlock &B) {
    
    /*
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
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
      Value *Operand = *Iter;

      if (Argument *Arg = dyn_cast<Argument>(Operand)) {
        outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
	       <<" DELLA FUNZIONE" << Arg->getParent()->getName()
               << "\n";
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
        outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue()
               << "\n";
      }
    }

    outs() << "LA LISTA DEI MIEI USERS:\n";
    for (auto Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";
    }

    outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
    for (auto Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) {
      outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
    }
    
    // Manipolazione delle istruzioni
    Instruction *NewInst = BinaryOperator::Create(
        Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0));

    NewInst->insertAfter(&Inst1st);
    // Si possono aggiornare le singole references separatamente?
    // Controlla la documentazione e prova a rispondere.
    Inst1st.replaceAllUsesWith(NewInst);
    */
  int index = 1;
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter){
    Instruction &Inst = *Iter;

    if(!strcmp(Inst.getOpcodeName(),"mul")){
      outs() << "Istruzione numero: " << index++ << "\n";
      outs() << Inst << "\n";
      outs() << "Operazione: " << Inst.getOpcodeName() << "\n";
      //outs() << "MOLTIPLICAZIONE" << "\n";
      for (auto *Iter_op = Inst.op_begin(); Iter_op != Inst.op_end(); ++Iter_op) {
        Value *Operand = *Iter_op;

        if (Argument *Arg = dyn_cast<Argument>(Operand)) {
          outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() <<" DELLA FUNZIONE" << Arg->getParent()->getName() << "\n";
        }
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
        outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue() << "\n";

        /*
        Questo serve perchè LLVM restituisce un intero forzato quando si usa getValue()
        per aggirare la cosa si usa una libreria che estrae l'intero e poi fa il cast a float
        questo serve perchè la funzione log2 prende un float come parametro.
        */
        uint64_t value = C->getValue().getZExtValue();  // extract the integer value from the APInt object
        float value_in_float = static_cast<float>(value);   
        float log_value = log2(value_in_float);
        //se il log_value è pari alla sua floor vuol dire che value_In_float è effettivamente una potenza di 2
        //se non lo fosse  log_value sarebbe un valore con la virgola e quindi non valido
        if(log_value == floor(log_value)){
          outs() << "Possiamo fare un miglioramento " << "\n";
        }
      }

    }
    /*
    for (auto *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
      Value *Operand = *Iter;
    */

  }
    
    


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




PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

