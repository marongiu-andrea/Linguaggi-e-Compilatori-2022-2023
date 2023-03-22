#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool isPowerOfTwo(ConstantInt *C) {
		return C->getValue().isPowerOf2();
	}

bool runOnBasicBlock(BasicBlock &B) {
    
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
    
	int index=-1;
	int pos_shift;
	int counter=0;
	std::vector <BinaryOperator*> mulToRemove;
	std::vector <BinaryOperator*> shiftToAdd;
    for (auto &instr : B) {
		if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
			if (BO->getOpcode() == Instruction::Mul){
				index=-1;
				ConstantInt *CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
				ConstantInt *CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
				if((CI1 && isPowerOfTwo(CI1)) || (CI2 && isPowerOfTwo(CI2))) {
					if (CI1 && isPowerOfTwo(CI1)){
						index=1;
						pos_shift=CI1->getValue().logBase2();
						outs() << pos_shift << "\n";
					}
					if (CI2 && isPowerOfTwo(CI2)){
						index=0; //vuol dire che la base dello shift e' l'altro operando
						pos_shift=CI2->getValue().logBase2();
						outs() << pos_shift << "\n";
					}
					outs() << "Sono arrivato fino a qui!\n";
					outs() << "Dopo aver controlalto entrambi gli IF!\n";
					if (index != -1){
						//Type* i32Ty = Type::getInt32Ty(context);
						Value* ShiftAmountValue = ConstantInt::get(Type::getInt32Ty(B.getContext()), pos_shift);
						BinaryOperator *NewInst = BinaryOperator::CreateShl(BO->getOperand(index),ShiftAmountValue,"",BO);
						shiftToAdd.push_back(NewInst);
						mulToRemove.push_back(BO);
						counter++;
						//BO->replaceAllUsesWith(NewInst);
						//BO->eraseFromParent();
					}
				}
			}
		}
    }
    
    for (int i=0; i<counter; i++) {
   		mulToRemove[i]->replaceAllUsesWith(shiftToAdd[i]);
   		//mulToRemove[i]->eraseFromParent();
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

