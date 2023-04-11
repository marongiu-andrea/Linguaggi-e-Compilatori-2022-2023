#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <vector>

using namespace llvm;

bool isPowerOfTwoMultiple(ConstantInt *C) {
		return C->getValue().isPowerOf2();
	}

int checkConstantIntPower2Multiple(ConstantInt* C1, ConstantInt* C2) {
    if (C1 && C1->getValue().isPowerOf2()) { //la costante numerica e' il primo operando
        return 0;
    }
    else if (C2 && C2->getValue().isPowerOf2()){ //la costante numerica e' il secondo operando
        return 1;
    }
    else {
        return -1;
    }
}

int checkConstantValueMultiple(ConstantInt* C1, ConstantInt* C2) {
    if (C1 && !C2) 
        return 0;
    if (C2 && !C1)
        return 1;
    return -1;
}

bool runOnBasicBlockMultiple(BasicBlock &B) {

    // Preleviamo le prime due istruzioni del BB
    Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());

    /* L'indirizzo della prima istruzione deve essere uguale a quello del 
       primo operando della seconda istruzione (per costruzione dell'esempio)
    */
    assert(&Inst1st == Inst2nd.getOperand(0));

    outs() << "PRIMA ISTRUZIONE: " << Inst1st << "\n";
    outs() << "COME OPERANDO: ";
    Inst1st.printAsOperand(outs(), false);
    outs() << "\n";

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
   
    Inst1st.replaceAllUsesWith(NewInst);
    Inst1st.eraseFromParent();



	ConstantInt* CI1;
	ConstantInt* CI2;
	APInt constantSub;
	APInt constantAdd;
	int constantIndexAdd=-1;
	int constantIndexSub=-1;
  std::vector<Instruction*> instrToRemove;
    for (auto &instr : B) {
          if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
            if(BO->getOpcode() == Instruction::Add){ // Verifico se si tratta di una ADD o meno
              CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
              CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
              constantIndexAdd = checkConstantValueMultiple(CI1,CI2); // salvo l'indice della costante all'interno dell'operazione
              if(constantIndexAdd != -1){
              	if(constantIndexAdd ==0 ){
              		constantAdd = CI1->getValue();
              	}
              	else {
              		constantAdd = CI2->getValue();
              	}
              	
		          for (auto instrUsed = BO->user_begin(); instrUsed != BO->user_end(); ++instrUsed){ //scorro tutti gli user della somma
		          	Instruction* userInstruction = dyn_cast<Instruction>(*instrUsed);
		          	if(auto *subInstr = dyn_cast<BinaryOperator>(userInstruction)){ // controllo se esiste un user che e' una BinaryOperator
		          		if(subInstr->getOpcode() == Instruction::Sub){// verifico se e' una sottrazione
		          			ConstantInt* Op2 = dyn_cast<ConstantInt>(subInstr->getOperand(1));
		          				if(Op2){
				      				constantSub = Op2->getValue(); // salvo il valore della costante di tipo APInt
				      				if(constantAdd == constantSub){ // se i due valori delle costanti coincidono	
                        instrToRemove.push_back(subInstr); //mi segno quale e' la prossima istruzione da rimuovere, non posso rimuoverla subito perchè in Foo.ll istruzioni possono dipendere da istruzioni contenute in questo vettore
						  				subInstr->replaceAllUsesWith(BO->getOperand(1-constantIndexAdd)); //sostituisco tutti gli usi con la variabile risultato della somma
						  				continue;
		          					}
		          				}
		          		}
		          	}
		          }
              }
            }
		  }
    }

	// rimuovo le istruzioni
    for (auto instr : instrToRemove) {
      instr->eraseFromParent();
    }

    return true;
  }


  bool runOnFunctionMultiple(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlockMultiple(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }







PreservedAnalyses MultiplePass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionMultiple(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}
