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
    Inst1st.eraseFromParent();


    
    //MARK: implementazione punto 3 della consegna

	ConstantInt* CI1;
	ConstantInt* CI2;
	Instruction* instrToRemove_;
	APInt constantSub;
	APInt constantAdd;
	int constantIndexAdd=-1;
	int constantIndexSub=-1;
	bool flag=false;
  std::vector<Instruction*> instrToRemove;
    for (auto &instr : B) {
          if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
            if(BO->getOpcode() == Instruction::Add){ //Verifico se si tratta di una ADD
              CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
              CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
              constantIndexAdd = checkConstantValueMultiple(CI1,CI2); //ottengo l'indice della costante all'interno dell'operazione
              if(constantIndexAdd != -1){
              	if(constantIndexAdd ==0 ){
              		constantAdd = CI1->getValue();
              	}
              	else {
              		constantAdd = CI2->getValue();
              	}
              	//outs() << constantAdd << "\n";
		          for (auto instrUsed = BO->user_begin(); instrUsed != BO->user_end(); ++instrUsed){ //scorro tutti gli user della somma
		          	if (flag==true){
    					instrToRemove_->eraseFromParent();
    					flag=false;
    				}
		          	Instruction* userInstruction = dyn_cast<Instruction>(*instrUsed);
		          	//outs() << userInstruction << "\n";
		          	if(auto *subInstr = dyn_cast<BinaryOperator>(userInstruction)){ //se sono sicuro di aver trovato uno user che e' una BinaryOperator
		          		if(subInstr->getOpcode() == Instruction::Sub){//verifico se e' una sottrazione
		          			ConstantInt* Op2 = dyn_cast<ConstantInt>(subInstr->getOperand(1)); //devo farlo cosi' per forza perche' tanto per avere la differenza la costante si trovera' sempre come secondo operando, nel caso ci sia
		          				if(Op2){ //se il cast e' andato a buon fine
				      				constantSub = Op2->getValue(); //prendo il valore della costante di tipo APInt
				      				if(constantAdd == constantSub){ //se i due valori delle costanti coincidono	
                        instrToRemove.push_back(subInstr); //mi segno quale e' la prossima istruzione da rimuovere, non posso rimuoverla subito perchè in Foo.ll istruzioni possono dipendere da istruzioni contenute in questo vettore
						  				subInstr->replaceAllUsesWith(BO->getOperand(1-constantIndexAdd)); //sostituisco tutti gli usi con la variabile risultato della somma
						  				flag=true;
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

