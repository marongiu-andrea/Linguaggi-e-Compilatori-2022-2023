#include "LocalOpts.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include <exception>
#include <string.h>
using namespace llvm;


bool runOnFunction(Function &F) {
  bool Transformed = false;

  return Transformed;
}

void unifyLoopsInductionVariables(Loop* current,Loop* next){
  PHINode* current_iv = current->getCanonicalInductionVariable();
  PHINode* next_iv = next->getCanonicalInductionVariable();
  next_iv->replaceAllUsesWith(current_iv);
  next_iv->eraseFromParent();
  /*printLoop(*current, outs());
  outs()<<"\n";
  printLoop(*next, outs());*/
}

void mergeLoops(Loop* current, Loop* next){
  BasicBlock* header_current = current->getHeader();
  BasicBlock* header_next = next->getHeader();

  BasicBlock* body_current = header_current->getTerminator()->getSuccessor(0);
  BasicBlock* body_next = header_next->getTerminator()->getSuccessor(0);

  BasicBlock* latch_current = current->getLoopLatch();
  BasicBlock* latch_next = next->getLoopLatch();

  BasicBlock* exit_next = next->getExitBlock();

  //connessioni
  body_current->getTerminator()->setSuccessor(0,body_next); //connetto il body di L1 al Body di L2
  header_current->getTerminator()->setSuccessor(1, exit_next); //connetto l'header di L1 all'exit di L2
  body_next->getTerminator()->setSuccessor(0, latch_current); //connetto il body di L2 al Latch di L1
  header_next->getTerminator()->setSuccessor(0, latch_next); //connetto l'header di L2 al latch di L2

  printLoop(*current, outs());
  outs()<<"\n******************NEXT**********************\n";
  printLoop(*next, outs());


}

PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F,FunctionAnalysisManager &AM) {
  auto &LI = AM.getResult<LoopAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  int l = 0;
  int BID = 0;
  SmallVector<Loop *, 4> PreOrderLoops = LI.getLoopsInPreorder();

  for(int i=0; i<PreOrderLoops.size()-1;i++){
    Loop* current = PreOrderLoops[i];
    Loop* next = PreOrderLoops[i+1];
    
    outs()<<"\n***************PROCESSING LOOP***************";
    BasicBlock *PH = current->getLoopPreheader();
    if(PH) outs()<<" "<<*PH<<"\n";
    if(current->getExitBlock() != next->getLoopPreheader())
      continue;

    outs() << "Adiacenti\n";
    
    //trip count
    if (SE.getSmallConstantTripCount(current) != SE.getSmallConstantTripCount(next))
      continue;
    
    outs() << "same trip\n";

    //control flow equivalence
    if (!(DT.dominates(current->getHeader(),next->getHeader()) && PDT.dominates(next->getHeader(),current->getHeader())))
      continue;
    
    outs() <<"control flow eq\n";

    unifyLoopsInductionVariables(current,next);
    mergeLoops(current, next);
    
  }
  
                                
  return PreservedAnalyses::none();
  }