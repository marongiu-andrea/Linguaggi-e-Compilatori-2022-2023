#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool isPowerOfTwoAlgebraic(ConstantInt *C) {
		return C->getValue().isPowerOf2();
	}

//MARK: passo di ottimizzazione 
bool runOnBasicBlockAlgebraic(BasicBlock &B) {
    
    
	int index=-1;
	int counter=0;
	std::vector <BinaryOperator*> mulToRemove;
	std::vector <BinaryOperator*> shiftToAdd;
    for (auto &instr : B) {
		if(auto *BO = dyn_cast<BinaryOperator>(&instr)){
			index=-1;
			if (BO->getOpcode() == Instruction::Mul){ //verifico se l'istruzione e' una mul
				ConstantInt *CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
				ConstantInt *CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
				if (CI1 && CI1->getValue().isOne()) {
					index=1;
				}
				else if (CI2 && CI2->getValue().isOne()) {
					index=0;
				}
				
				if (index != -1) {
					BO->replaceAllUsesWith(BO->getOperand(index)); //sostituisco gli usi della attuale istruzione con quelli dell'operando NON COSTANTE
				}
			}
			
			else if (BO->getOpcode() == Instruction::Add) {
				ConstantInt *CI1 = dyn_cast<ConstantInt>(BO->getOperand(0));
				ConstantInt *CI2 = dyn_cast<ConstantInt>(BO->getOperand(1));
				if (CI1 && CI1->getValue().isZero()) {
					index=1;
				}
				else if (CI2 && CI2->getValue().isZero()) {
					index=0;
				}
				
				if (index != -1) {
					BO->replaceAllUsesWith(BO->getOperand(index));//faccio la stessa identica cosa che ho fatto sopra
				}
			}
		}
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
  
	





PreservedAnalyses AlgebraicPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionAlgebraic(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

