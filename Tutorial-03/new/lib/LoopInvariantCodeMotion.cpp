#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;
#include <iostream>

#include <map>

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
    outs() << "\nLOOP-INVARIANT CODE MOTION INIZIATO...\n";

    // Dominance tree.
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    // Mappa delle istruzioni.
    std::map <Instruction&, bool> LIMap;

    // Parte 1 - trovare le istruzioni loop-invariant.
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
        Instruction& I = *iter_inst;
				bool invariant = true;

        if (I.isBinaryOp()) {
					for (auto operand = I.operands().begin(); operand != I.operands().end(); ++operand) {
						ConstantInt *C = dyn_cast<ConstantInt>(operand);
						Instruction *Inst = dyn_cast<Instruction>(operand);

						if (C)
							continue;
						else if (LIMap[*Inst] || L->contains(Inst->getParent()))
							continue;
						else
							invariant = false;
					}
        }

				if (invariant)
					LIMap[I] = true;
      }
    }

		

    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

