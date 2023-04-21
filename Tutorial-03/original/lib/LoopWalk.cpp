#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;
#include <iostream>

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n";

    // Es.1 pt.1.
    if (L->isLoopSimplifyForm())
      std::cout << "Il loop è normalizzato." << std::endl;
    else
      std::cout << "Il loop non è normalizzato." << std::endl;

    // Es.1 pt.2.
    llvm::BasicBlock *PH = L->getLoopPreheader();
    if (PH)
      std::cout << "Il loop ha un preheader." << std::endl;
    else
      std::cout << "Il loop non ha un preheader." << std::endl;

    // Es.1 pt.3 + Es.2pt. 1,2,3,4.
    std::cout << "Stampa di tuttie le Instructions per tutti i basic block del loop:"
              << std::endl << std::endl;
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      for(auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
			  Instruction& I = *iter_inst;
        outs() << I;
        std::string name = I.getOpcodeName();

        if (name == "sub") {
          std::cout << "  <-- Un'struzione sub è qui!" << std::endl;
          std::cout << "  L'operando/gli operandi della sub viene/vengono definito/i in:"
                    << std::endl;

          for (auto *iter_op = I.op_begin(); iter_op != I.op_end(); ++iter_op) {
            Value *Operand = *iter_op;
            ConstantInt *C = dyn_cast<ConstantInt>(Operand);

            if (!C) {
              outs() << *Operand;
              std::cout << std::endl;
              std::cout << "  Che appartiene al basic block:" << std::endl;
              Instruction *Inst = dyn_cast<Instruction, Value>(Operand);
              outs() << *Inst->getParent();

              std::cout << std::endl << std::endl;
            }
          }

          std::cout << "  Continua l'analisi dal punto in cui si era interrotta..."
                    << std::endl;
        }
        else
          std::cout << std::endl;
      }

      std::cout << std::endl;
    }

    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

