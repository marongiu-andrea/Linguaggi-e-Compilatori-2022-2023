#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

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
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    outs() << "\nLOOPPASS INIZIATO...\n"; 
    if (L->isLoopSimplifyForm())
        outs() << "true\n";
      else
        outs() << "false\n";
    
    
    BasicBlock* preheder = L->getLoopPreheader();
    // LoopPass* simplify = isLoopSimplifyForm();
    if(preheder){
      outs() << "Il loop ha un preheader:\n\nIstruzioni Preheader\n";
    }  
    return false; 
  }
};


char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

