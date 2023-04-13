#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n"; 

    // verificare la forma normalizzata
    if (L->isLoopSimplifyForm()){
      outs() << "\nLoop in forma normalizzata\n";
      
      // controllo preheader
      BasicBlock *B1 = L->getLoopPreheader();
      outs() << "\nPreheader del loop: " <<  *B1 << "\n";

      // stampo i basic blocks del loop
      int i = 1;
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){

        BasicBlock *BB = *BI;

        outs() << "\nBasic block n. " << i << ": " << *BB << "\n";
        i++;
      }

    }
    
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

