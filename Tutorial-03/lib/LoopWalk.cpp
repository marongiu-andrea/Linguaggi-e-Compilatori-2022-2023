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
    else
      return false; 

    for(auto &bb: L->getBlocks()){
      for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
          Instruction* ii = &*i;
          // errs() << *ii << "\n";
            if(ii->getOpcode() == Instruction::Sub){
              outs() << *ii << "\n\n";
              outs() << "Basic Block della suddetta istruzione:";
              outs() << *ii->getParent() << "\n";
            }
      }
    }

  }
};


char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

