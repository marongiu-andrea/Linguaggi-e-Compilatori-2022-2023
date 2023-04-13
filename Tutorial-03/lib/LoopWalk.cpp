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
    //verifico se il loop e' nella forma normale/semplificata
    if (L->isLoopSimplifyForm()) {
      outs() << "Il Loop e' nella forma normale!\n\n";
    }
    //prendo il blocco di preheader e lo stampo
    BasicBlock *preheader = L->getLoopPreheader();
    if (preheader){
      outs() << "Il loop contiene un blocco di preheader!\n";
      preheader->print(outs());
      outs() << "\n";
    }
    //scorro tutti i BB che compongono il loop e li stampo
    BasicBlock *actualBB;
    outs() << "Procedo alla stampa dei BB che compongono il loop:\n\n";
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      actualBB = *BI;
      if(actualBB) {
        actualBB->print(outs());
        outs() << "\n";
      }
    }
    return false;
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

