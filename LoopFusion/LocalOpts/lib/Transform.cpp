#include "LocalOpts.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include <string.h>
using namespace llvm;


bool runOnFunction(Function &F) {
  bool Transformed = false;

  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F,FunctionAnalysisManager &AM) {
  auto &LI = AM.getResult<LoopAnalysis>(F);
  int l = 0;
  int BID = 0;
  SmallVector<Loop *, 4> PreOrderLoops = LI.getLoopsInPreorder();
  for(int i=0; i<PreOrderLoops.size()-1;i++){
    Loop* current = PreOrderLoops[i];
    Loop* next = PreOrderLoops[i+1];

    outs()<<"\n***************PROCESSING LOOP***************";
    BasicBlock *PH = current->getLoopPreheader();
    if(PH) outs()<<" "<<*PH<<"\n";
    if(current->getExitBlock()==next->getLoopPreheader())
      outs()<< *current << " \n è adiacente a \n" << *next <<"\n";
  }
/*
  bool firstRound = true;
  BasicBlock *previousExitBlock;
  
  for(Loop *L : PreOrderLoops){
    outs()<<"\n***************PROCESSING LOOP***************";
    BasicBlock *PH = L->getLoopPreheader();
    if(PH) outs()<<" "<<*PH<<"\n";

    if(!firstRound && L) {
      if(BasicBlock* head = L->getHeader()){
        if(head==previousExitBlock){
          outs()<< "adiacente a "<< head;
        }
      }
    }

    previousExitBlock = L->getExitBlock();
    if(firstRound) {
      firstRound = false;
      continue;
    }
    
  }*/
                                
  return PreservedAnalyses::none();
  }