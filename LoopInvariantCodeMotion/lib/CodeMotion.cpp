#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/Use.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <vector>
#include <map>
#include "llvm/IR/IRBuilder.h"

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
    //utilizzando Use faccio senza fare il controllo delle costanti, riducendo così la quantità di lavoro
    
    for(Use &use : inst->operands()){

      if(isa<BranchInst>(inst))
        return false;

      if(isa<PHINode>(inst))
        return false;

      Value *val = use.get();
      if(Instruction *DefInst = dyn_cast<Instruction>(val)){
        if (loop->contains(DefInst)){
          return isLoopInvariant(DefInst,loop);
        }
      }
    }
    return true;
  }



  virtual SmallVector<BasicBlock*> getExits(Loop* loop){
    SmallVector<BasicBlock*> exitBlocks;
    for(Loop::block_iterator iter = loop->block_begin(); iter != loop->block_end(); ++iter){
      if(loop->isLoopExiting(*iter)){
        exitBlocks.push_back(*iter);
      }
    }
    return exitBlocks;
  }

  virtual bool dominatesAllExits(Instruction *inst, DominatorTree *DT, SmallVector<BasicBlock*> exitBlocks){
    BasicBlock* father = inst->getParent();
    for(auto BB : exitBlocks){
      if(DT->dominates(father,BB))
        continue;
      else
       return false;
    }
    return true;
  }

  virtual bool dominatesInstrUsage(Instruction *inst, DominatorTree *DT){
    for(auto use = inst->user_begin(); use != inst->user_end(); ++use){
      if(DT->dominates(inst,dyn_cast<Instruction>(*use)))
        continue;
      else
       return false;
    }
    return true;
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    SmallVector<BasicBlock*> exitBlocks = getExits(L);
    std::vector<Instruction*> movableInst;
    /*for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();++BI) {
          BasicBlock *B = *BI;
          for (auto iter = B->begin(); iter != B->end(); ++iter) {
            (*iter).print(outs());
            outs()<<" : "<<isLoopInvariant(&(*iter),L)<<" \n";
          }
    }*/

    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();++BI) {
          BasicBlock *B = *BI;
          for (auto iter = B->begin(); iter != B->end(); ++iter) {
            if (isLoopInvariant(&(*iter), L)) {
              if(dominatesAllExits(&(*iter),DT,exitBlocks) && dominatesInstrUsage(&(*iter),DT)){
                (*iter).print(outs());
                outs()<<" HOISTABLE \n";
                LLVMContext &C = iter->getContext();
                MDNode *N = MDNode::get(C,MDString::get(C,"hoistable"));
                (*iter).setMetadata("HOIST",N);
                Instruction* inst = dyn_cast<Instruction>(iter);
                movableInst.push_back(inst);
              }
            }
          }
    }

    //spostamento istruzioni
    BasicBlock* preHeader = L->getLoopPreheader();
    /*for(Instruction* inst = movableInst.begin();inst != movableInst.end(); ++inst){
      inst->moveBefore(preHeader->getTerminator());
    }*/

    for(auto inst : movableInst){
      inst->moveBefore(preHeader->getTerminator());
    }
    return false;
  }
};

char CodeMotionPass::ID = 0;
RegisterPass<CodeMotionPass> X("code-motion", "Loop Code Motion");

} // anonymous namespace
