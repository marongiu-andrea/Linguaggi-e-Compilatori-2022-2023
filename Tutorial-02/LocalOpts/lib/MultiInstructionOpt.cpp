#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Instruction.h"
#include "vector"

using namespace llvm;

//controllo se il tipo passato è un operazione a due operandi algebrica
bool isAlgebric(unsigned tipoistr){
  switch (tipoistr){
  case Instruction::Add :
  case Instruction::Sub :
  case Instruction::Mul :
  case Instruction::SDiv :  
    return true;
    break;
  default:
    break;
  }

  return false;
}
//controllo che la seconda istruzione sia del "tipo" opposto della prima: add - sub; mul - Sdiv
bool isOpOpposta(unsigned primaInstr, unsigned secondaInstr){
  bool risposta = false;
  //Scrivo un codive prolisso, ma chiaro da leggere
  if(primaInstr == Instruction::Add && secondaInstr == Instruction::Sub)
    risposta = true;
  if(primaInstr == Instruction::Sub && secondaInstr == Instruction::Add)
    risposta = true;
  if(primaInstr == Instruction::Add && secondaInstr == Instruction::Sub)
    risposta = true;
  if(primaInstr == Instruction::Mul && secondaInstr == Instruction::SDiv)
    risposta = true;
  if(primaInstr == Instruction::SDiv && secondaInstr == Instruction::Mul)
    risposta = true;
  
  return risposta;
}

bool runOnBasicBlockMultiInstr(BasicBlock &B) {
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
    bool debug = true;
    int theNumb, numeroNumb, valoreIntero;
    numeroNumb = 0;
    Value* op0;
    Value* op1;
    Value* opReg;
    unsigned tipoistr;
    

    for (Instruction &iter : B){
      theNumb = -1;
      numeroNumb = 0;
      tipoistr = iter.getOpcode();
      //Controllo elemento neutro somma/sottrazione
      if(isAlgebric(tipoistr)){
          if(debug)
            printf("E' una algebrica\n");

          op0 = iter.getOperand(0);
          op1 = iter.getOperand(1);
          if(debug)
            printf("Ho gli operandi\n");

          //Controllo gli operandi, selezioni il valore numerico se c'è e preparo un puntatore
          //opReg al registro da utilizzare dopo se un operatore è un numero e l'altro è un registro
          if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(0))){
            theNumb=0;
            valoreIntero= C->getSExtValue();
            opReg=op1;
            if(debug)
              printf("Ho controllato il primo operando\n");

            numeroNumb++;
          }else if(ConstantInt *C = dyn_cast<ConstantInt>(iter.getOperand(1))){
            theNumb=1;
            valoreIntero= C->getSExtValue();
            opReg=op0;
            if(debug)
              printf("Ho controllato il secondo operando\n");
            
            numeroNumb++;
          }
        
        //Controllo ci sia un numerico e un registro altrimenti non faccio nulla
        if(numeroNumb != 1)
          continue;
        else{
          //Solo uno dei due è numerico, ora devo controllare tutti i suoi utilizzatori
          if(debug)
            printf("Itero sugli utilizzatori\n");
          
          if(Instruction* I = &iter){
            for(auto uso = I->user_begin(); uso != I->user_end(); ++uso){
              //se riesco a trattare l'utilizzatore come un istruzione
              //Devo controllare se l'istruzione ha un operando numerico, e nel caso se è lo stesso
              //intero dell'uso di riferimento
              //se è così allora posso propagare opReg al posto del valore del nuovo utilizzatore
              //a=b+1, c=a-1 > a=b+1; c=b

              //controllo che sia l'operazione opposta all'istruzione
              //Sta roba non funziona e che cazzo???
              if(Instruction *userInst = dyn_cast<Instruction>(uso)){

              }
              

            }
          }*/
        }
      }

          
        
    };
    /*for(int i=0; i < daEliminare.size(); i++){
      daEliminare[i]->eraseFromParent();
    }*/

    return true;
  }


  bool runOnFunctionMultiInstr(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlockMultiInstr(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




PreservedAnalyses MultiInstrPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionMultiInstr(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

