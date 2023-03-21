#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "cmath"

using namespace llvm;

bool isPoweOfTwo(int n){
  if(n>0)
    return (n & (n-1)) == 0;
  else
    return false;
}

bool runOnBasicBlock(BasicBlock &B) {
    /*
    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

    // L'indirizzo della prima istruzione deve essere uguale a quello del 
    // primo operando della seconda istruzione (per costruzione dell'esempio)
    assert(&Inst1st == Inst2nd.getOperand(0));

    // Stampa la prima istruzione
    outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
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
    for (Instruction &iter : B){
      if(iter.getOpcode() == Instruction::Mul){
          int operandPow2=-1, operandNotPow=-1;
          int value0,value1;
          double valueF;
          bool trasformato = false;
          //Estraggo gli operandi in forma numerica
          if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))) {
            value0 = C->getSExtValue();
            trasformato = true;  
          }
          if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))) {
            value1 = C->getSExtValue();
            trasformato=true;  
          }
          if(!trasformato)
            continue;
          //controllo se uno dei due è potenza di 2
          if(isPoweOfTwo(value0)){
              operandPow2=0;
              operandNotPow=1;
              printf("Il primo operando numerico è potenza di 2: %i\n",value0);
              valueF=value0;
            }else if (isPoweOfTwo(value1)){
              operandPow2=1;
              operandNotPow=0;
              printf("Il secondo operando numerico è potenza di 2: %i\n",value1);
              valueF=value1;
            }else{
              //nessuno dei due operandi è potenza di 2
              continue;
            }
            //creo l'operando per la nuova operazione di shift
            printf("Sono qui\n");
            ConstantInt *Con = dyn_cast<ConstantInt>(iter.getOperand(operandPow2));
            printf("Sono qui\n");
            Value *newOP = ConstantInt::get(Con->getType(),
              static_cast<int>(log2(valueF)));
            printf("Sono qui\n");
            Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, 
              iter.getOperand(operandNotPow), 
              newOP);
              printf("Sono qui\n");

            //sostituisco la vecchio mul con la nuova shift
            NewInst->insertAfter(&iter);
            //iter.replaceAllUsesWith(NewInst);
            //iter.eraseFromParent();

      }
    };

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

