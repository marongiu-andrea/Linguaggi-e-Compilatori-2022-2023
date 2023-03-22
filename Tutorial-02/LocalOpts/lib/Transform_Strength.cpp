#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool isPowerOfTwoStrength(ConstantInt *C) {
		return C->getValue().isPowerOf2();
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
    
	ConstantInt* CI1;
	ConstantInt* CI2;
	Instruction* instrToRemove;
	bool flag;
    for (auto &instr : B) {
    	if (flag==true){
    		instrToRemove->eraseFromParent();
    		flag=false;
    	}
		if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
			if(BO->getOpcode() == Instruction::SDiv){ //se si tratta di una operazione che posso sostituire con un RSHIFT
				outs() << "Sono arrivato fino a qui!\n";
				CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
				if(CI2 && isPowerOfTwoStrength(CI2)) {
					flag=true;
					instrToRemove=&instr;
					Value* ShiftAmount = ConstantInt::get(Type::getInt32Ty(B.getContext()), CI2->getValue().logBase2());
					BinaryOperator* NewInst = BinaryOperator::CreateAShr(BO->getOperand(0),ShiftAmount,"",BO);
					BO->replaceAllUsesWith(NewInst);
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

