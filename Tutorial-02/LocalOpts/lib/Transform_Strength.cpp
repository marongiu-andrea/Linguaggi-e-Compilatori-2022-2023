#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool isPowerOfTwoStrength(ConstantInt *C) {
		return C->getValue().isPowerOf2();
	}

int checkConstantIntPower2(ConstantInt* C1, ConstantInt* C2) {
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

int checkConstantValue(ConstantInt* C1, ConstantInt* C2) {
    if (C1 && !C2) 
        return 0;
    if (C2 && !C1)
        return 1;
    return -1;
}

bool runOnBasicBlockStrength(BasicBlock &B) {
    
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


    
    //MARK: implementazione punto 2 della consegna

	ConstantInt* CI1;
	ConstantInt* CI2;
	Instruction* instrToRemove;
	bool flag=false;
    for (auto &instr : B) {
    	if (flag==true){
    		instrToRemove->eraseFromParent();
    		flag=false;
    	}
          if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
            if(BO->getOpcode() == Instruction::SDiv){ //se si tratta di una operazione che posso sostituire con un RSHIFT
              CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
              if(CI2 && isPowerOfTwoStrength(CI2)) {
                flag=true;
                instrToRemove=&instr;
                Value* ShiftAmount = ConstantInt::get(Type::getInt32Ty(B.getContext()), CI2->getValue().logBase2());
                BinaryOperator* NewInst = BinaryOperator::CreateAShr(BO->getOperand(0),ShiftAmount,"",BO);
                BO->replaceAllUsesWith(NewInst);
              }
            }
            else if (BO->getOpcode() == Instruction::Mul) {
                int constantIndex=-1;
                int pos_shift;
                CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
                CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
                constantIndex = checkConstantIntPower2(CI1,CI2);
                if (constantIndex != -1) { //sono sicuro di aver trovato una istruzione di moltiplicazione che puo' essere sostituita usando una shift a sx
                    flag=true;
                    if (constantIndex==0) {
                        pos_shift = CI1->getValue().logBase2();
                    }
                    else {
                        pos_shift = CI2->getValue().logBase2();
                    }
                    Value* ShiftAmountValue = ConstantInt::get(Type::getInt32Ty(B.getContext()), pos_shift);
                    Instruction* NewInst = BinaryOperator::CreateShl(BO->getOperand(1-constantIndex),ShiftAmountValue,"",BO);
                    BO->replaceAllUsesWith(NewInst);
                    instrToRemove=&instr;
                }
                else { //devo valutare se sono dentro ad un caso di quelli in cui sto moltiplicando per il valore precedente a quello di una potenza di 2
                    constantIndex=checkConstantValue(CI1,CI2);
                    if (constantIndex != -1) {
                        APInt value=APInt(32,1,true);
                        int valueInt;
                        int operandInt;
                        if (constantIndex == 0) {
                            pos_shift = CI1->getValue().nearestLogBase2();
                            //value = CI1->getValue();
                            operandInt=CI1->getValue().getSExtValue();
                        }
                        else {
                            pos_shift = CI2->getValue().nearestLogBase2();
                            //value = CI2->getValue();
                            operandInt=CI2->getValue().getSExtValue();
                        }
                        value=value.shl(pos_shift); //value adesso contiene il valore della potenza di 2 piu' vicina alla costante numerica della moltiplicazione
                        valueInt = value.getSExtValue();
                        //outs() << valueInt << " " << operandInt << "\n";
                        if ((valueInt-operandInt) == 1) {
                            //outs() << "Sono arrivato fino a qui\n";
                            Instruction* NewSub = BinaryOperator::CreateSub(ConstantInt::get(Type::getInt32Ty(B.getContext()), valueInt),BO->getOperand(1-constantIndex),"",BO);
                            BO->replaceAllUsesWith(NewSub);
                            flag=true;
                            instrToRemove=&instr;
                            //TODO: manca da implementare la logica di generazione dell'istruzione di shift, la logica dell'istruzione di sottrazione e poi bisogna sostituire tutte gli user della moltiplicazione CON L'OPERAZIONE DI SOTTRAZIONE
                        }
                    }
                }
            }
		  }
    }

    return true;
  }


  bool runOnFunctionStrength(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runOnBasicBlockStrength(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }
  
	





PreservedAnalyses StrengthPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionStrength(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

