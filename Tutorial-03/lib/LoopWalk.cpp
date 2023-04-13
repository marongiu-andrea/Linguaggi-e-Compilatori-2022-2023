#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Analysis/LoopInfo.h>

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
    if (L->isLoopSimplifyForm())
        outs() << "true\n";
      else
        outs() << "false\n";
    
    
    BasicBlock* preheder = L->getLoopPreheader();
    // LoopPass* simplify = isLoopSimplifyForm();
    if(preheder){
      outs() << "Il loop ha un preheader:\nIstruzioni Preheader\n";
    }  
    return false; 
  }
};


char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

