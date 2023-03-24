#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "vector"

using namespace llvm;

bool runOnBasicBlockAlgebraic(BasicBlock &B) {
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
   //variabile di posizione dell'operando neutro
   std::vector<Instruction*> daEliminare;
    int neutro;
    Value* op0;
    Value* op1;
    for (Instruction &iter : B){
      neutro = -1;
      //Controllo elemento neutro somma/sottrazione
        if(iter.getOpcode() == Instruction::Add || iter.getOpcode() == Instruction::Sub){
            printf("E' una somma/sottra\n");
            op0 = iter.getOperand(0);
            op1 = iter.getOperand(1);
            printf("Ho gli operandi\n");
            if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){
              if(C->getValue().isZero()){
                neutro=0;
              }
              printf("Ho controllato il primo operando\n");
            }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){
              if(C->getValue().isZero()){
                neutro=1;
              }
              printf("Ho controllato il secondo operando\n");
            }
            printf("L'operando a zero è il %i\n",neutro);
          
          }
          if(iter.getOpcode() == Instruction::Mul || iter.getOpcode() == Instruction::SDiv){
            printf("E' una somma/sottra\n");
            op0 = iter.getOperand(0);
            op1 = iter.getOperand(1);
            printf("Ho gli operandi\n");
            if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){
              if(C->getValue().isOne()){
                neutro=0;
              }
              printf("Ho controllato il primo operando\n");
            }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){
              if(C->getValue().isOne()){
                neutro=1;
              }
              printf("Ho controllato il secondo operando\n");
            }
            printf("L'operando a uno è il %i\n",neutro);
               
          }
          if(neutro != -1){
            iter.replaceAllUsesWith(iter.getOperand(1-neutro));
            daEliminare.push_back(&iter);
            printf("Ho sostituito l'operando\n");        
          } 
        
    };
    for(int i=0; i < daEliminare.size(); i++){
      daEliminare[i]->eraseFromParent();
    }

    return true;
  }


  bool runOnFunctionAlgebraic(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlockAlgebraic(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionAlgebraic(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

