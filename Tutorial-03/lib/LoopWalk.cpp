#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
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
    if(L->isLoopSimplifyForm()){
      outs() <<"FORMA NORMALIZZATA\n";
    }
    BasicBlock *BB = L->getLoopPreheader();
    if(BB){
      outs() << "PREHEADER " << *BB << "\n";
    }
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *B = *BI;
      for (auto Iter = B->begin(); Iter != B->end(); ++Iter){
        if(!strcmp(Iter->getOpcodeName(), "sub")){
          outs() << *Iter << " ###QUESTA E UNA SUB###\n";
        }else{
          outs() << *Iter << "\n";
        }
      }
      outs() << "\n\n";
      
    }
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

