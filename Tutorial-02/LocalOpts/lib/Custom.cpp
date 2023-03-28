#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <stack>
#include <math.h>

using namespace llvm;


bool TransformPass::runOnBasicBlock(BasicBlock &BB) {
  // Itera attraverso le istruzioni del basic block.
  for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {
    Instruction& I = *iter_inst;

    // Se l'operazione ha un operatore binario.
    if (auto *op = dyn_cast<BinaryOperator>(&*iter_inst)) {
      std::string opcd = iter_inst->getOpcodeName();

      // Se si tratta di una moltiplicazione.
      if (opcd == "mul") {
        // Stack per i valori associati agli shift.
        std::stack<int> shifts;
        // Operando da shiftare.
        Value *Shiftee;

        // Itera attraverso gli operandi dell'istruzione.
        for (auto *iter_op = I.op_begin(); iter_op != I.op_end(); ++iter_op) {
          Value *OP = *iter_op;
          bool do_shift = false;
          
          // Se l'operando è una costante numerica intera.
          if (ConstantInt *C = dyn_cast<ConstantInt>(OP)) {
            int val = C->getSExtValue();
            // Se la costante è una potenza del 2, la aggiunge allo stack.
            if ((val & (val - 1)) == 0) {
              shifts.push(log2(val));
              do_shift = true;
            }
          }

          // Se non si tratta di una costante potenza del 2, l'operando è
          // candidato per essere shiftato.
          if (!do_shift) {
            Shiftee = OP;
          }
        }

        // Se è possibile eseguire uno o più shift.
        if (shifts.size() != 0) {
          // Se entrambi i valori erano costanti e potenze del 2, si deve
          // di fatto eseguire uno shift su 1.
          if (shifts.size() == 2) {
            Shiftee = ConstantInt::get(Type::getInt32Ty(BB.getContext()), 1);
          }

          // Si calcola il valore intero associato allo shift complessivo.
          int tot_shift = 0;
          while (!shifts.empty()){
            tot_shift += shifts.top();
            shifts.pop();
          }
          // Lo si converte a Value*.
          Value *shiftop = ConstantInt::get(Type::getInt32Ty(BB.getContext()), tot_shift);

          // Si crea la nuova istruzione.
          Instruction *NewInst = BinaryOperator::Create(
              Instruction::Shl, Shiftee, shiftop
              );
          // Si inserisce la nuova istruzione subito dopo quella corrente.
          NewInst->insertAfter(&I);
          // Si rimpiazzano tutti gli usi della corrente con la nuova.
          I.replaceAllUsesWith(NewInst);
          // Riguardo alla domanda:
          // Si possono aggiornare le singole references separatamente?
          // La risposta è: sì, come già visto sulle slides è possibile iterare
          // sugli Uses, modificandoli secondo le esigenze. Poi, si può usare
          // il metodo:
          // void replaceUsesWithIf (Value *New, llvm::function_ref<bool(Use &U)> ShouldReplace)
          // per aggiornare references in base ad una data condizione.
        }
      }
    }
  }

  return true;
}


bool TransformPass::runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
  // Un semplice passo di esempio di manipolazione della IR.
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}