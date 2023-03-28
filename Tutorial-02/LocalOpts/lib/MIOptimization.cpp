#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <stack>
#include <math.h>

using namespace llvm;


bool MIOptimizationPass::runOnBasicBlock(BasicBlock &BB) {
  // Itera attraverso le istruzioni del basic block.
  for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {
    Instruction& I = *iter_inst;
		std::string opcd1 = I.getOpcodeName();

    // Se l'istruzione ha un operatore binario ed è add/sub.
    if (I.isBinaryOp() && (opcd1 == "add" || opcd1 == "sub")) {
      ConstantInt *C1 = dyn_cast<ConstantInt>(I.getOperand(1));

      // Se il secondo operando è un numero costante.
      if (C1) {
        // Itera attraverso gli user dell'istruzione.
        for (auto iter_user = I.user_begin(); iter_user != I.user_end(); ++iter_user) {
          Instruction *U = dyn_cast<Instruction>(*iter_user);
          std::string opcd2 = U->getOpcodeName();

          // Se lo user corrente ha un operatore binario ed è add/sub.
          if (U->isBinaryOp() && (opcd2 == "add" || opcd2 == "sub")) {
            ConstantInt *C2 = dyn_cast<ConstantInt>(U->getOperand(1));

            // Se anche in questo caso il secondo operando è costante.
            if (C2) {
              // Se il secondo operando è uguale in entrambe le istruzioni e
              // le due istruzioni sono diverse (una ad ed una sub).
              if (C1 == C2 && opcd1 != opcd2) {
                U->replaceAllUsesWith(U->getOperand(0));
              }
            }
          }
        }
      }
		}
	}

  return true;
}


bool MIOptimizationPass::runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses MIOptimizationPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
  // Un semplice passo di esempio di manipolazione della IR.
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}