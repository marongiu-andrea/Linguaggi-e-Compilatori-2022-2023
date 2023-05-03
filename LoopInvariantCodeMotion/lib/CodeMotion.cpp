#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <vector>
#include <map>

using namespace llvm;

namespace {

class CodeMotionPass final : public LoopPass {
public:
  static char ID;

  CodeMotionPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }


  virtual bool isLoopInvariant(Instruction *inst, Loop* loop){
    //calcolo reaching definitions
    //utilizzando Use faccio senza fare il controllo delle costanti, riducendo così la quantità di lavoro
    for(Use &uses : inst->operands()){
      if(isa<BranchInst>(inst))
        return false;
      Value *val = uses.get();
      if(Instruction *DefInst = dyn_cast<Instruction>(val)){
        if (loop->contains(DefInst)){
          return false;
        }
      }
    }
    return true;
  }

  virtual void isSafeToMove(){

  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();++BI) {
          BasicBlock *B = *BI;
          for (auto iter = B->begin(); iter != B->end(); ++iter) {
            (*iter).print(outs());
            outs()<<": "<<isLoopInvariant(&(*iter),L)<<" \n";
          }
    }
    return false;
  }
};

char CodeMotionPass::ID = 0;
RegisterPass<CodeMotionPass> X("code-motion", "Loop Code Motion");

} // anonymous namespace
