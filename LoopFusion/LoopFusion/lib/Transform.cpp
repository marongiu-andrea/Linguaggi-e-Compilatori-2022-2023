#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/PassManager.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include <map>

using namespace llvm;

void fondiLoop(Loop *, Loop *);

PreservedAnalyses Mem2RegPass::run([[maybe_unused]] /*Module*/ Function &F,
                                             FunctionAnalysisManager &AM) {

  // Un semplice passo di esempio di manipolazione della IR
  auto &LI = AM.getResult<LoopAnalysis>(F);
  std::map<Loop*,BasicBlock*> preheaders;
  llvm::Loop* l_prev = nullptr; //puntatore al loop precedente
  BasicBlock* pre;
  BasicBlock* ext;
  Instruction* inst;

  auto loops = LI.getLoopsInPreorder();
  for (llvm::Loop * l_actual : loops) {
    if(l_prev) {
      
      pre = l_actual->getLoopPreheader();
      ext = l_prev->getExitBlock();
      if(pre == ext){
        inst = dyn_cast<Instruction>(pre->begin());
        if (llvm::BranchInst* branchInst = dyn_cast<BranchInst>(inst)) {
          fondiLoop(l_actual, l_prev);
        }
      }
    }
    else{
      l_prev = l_actual; //aggiorno il puntatore al loop precedente
    }
  }
  return PreservedAnalyses::none();
}


void fondiLoop(Loop* attuale, Loop* prev) {
  //collego il latch del loop attuale all'header del loop precedente
  BasicBlock* header_prev = prev->getHeader();
  BasicBlock* latch_actual = attuale->getLoopLatch();
  BranchInst* latch_actual_bi = dyn_cast<BranchInst>(latch_actual->getTerminator());
  latch_actual_bi->setSuccessor(0,header_prev);
  //collego l'exiting del loop precedente all'header del loop attuale
  BasicBlock* exiting_prev = prev->getExitingBlock();
  BasicBlock* header_actual = attuale->getHeader();
  BranchInst *exiting_prev_bi = dyn_cast<BranchInst>(exiting_prev->getTerminator());
  exiting_prev_bi->setSuccessor(0,header_actual);
  //collego il latch del precedente all'header attuale
  BasicBlock* latch_prev = prev->getLoopLatch();
  BranchInst* latch_prev_bi = dyn_cast<BranchInst>(latch_prev->getTerminator());
  latch_prev_bi->setSuccessor(0,header_actual);
}

