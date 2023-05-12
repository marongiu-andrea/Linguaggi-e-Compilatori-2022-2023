#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/PassManager.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include <map>

using namespace llvm;

void fondiLoop(Loop *, Loop *, ScalarEvolution &);
bool checkAdiacenza(Loop* l_actual, Loop* l_prev);
bool checkBranch(Loop* l_actual);
bool checkCFEquivalent(Loop* l_actual, Loop* l_prev, DominatorTree&, PostDominatorTree&);
bool checkLoopTripCount(Loop* attuale, Loop* prev, ScalarEvolution &SE);

//TODO: Implementa il controllo della cfg equivalenza

PreservedAnalyses LoopFusionPass::run([[maybe_unused]] /*Module*/ Function &F,
                                             FunctionAnalysisManager &AM) {

  // Un semplice passo di esempio di manipolazione della IR
  auto &LI = AM.getResult<LoopAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  std::map<Loop*,BasicBlock*> preheaders;
  llvm::Loop* l_prev = nullptr; //puntatore al loop precedente
  Instruction* inst;

  auto loops = LI.getLoopsInPreorder();
  for (llvm::Loop * l_actual : loops) {
    if(l_prev) {
      if(checkAdiacenza(l_actual, l_prev) && checkBranch(l_actual) && checkLoopTripCount(l_actual,l_prev,SE) && checkCFEquivalent(l_actual, l_prev,DT,PDT)){ //verifico le varie condizioni
        fondiLoop(l_actual, l_prev,SE);
      }
    }
    else{
      l_prev = l_actual; //aggiorno il puntatore al loop precedente
    }
  }
  return PreservedAnalyses::none();
}


void fondiLoop(Loop* attuale, Loop* prev, ScalarEvolution &SE) {

  BasicBlock* header_prev = prev->getHeader();
  BasicBlock* header_actual = attuale->getHeader();
  BasicBlock* body_actual = (dyn_cast<BranchInst>(header_actual->getTerminator()))->getSuccessor(0);
  BasicBlock* body_prev = (dyn_cast<BranchInst>(header_prev->getTerminator()))->getSuccessor(0);
  BasicBlock* exiting_actual = attuale->getExitingBlock(); 
  BasicBlock* latch_prev = prev->getLoopLatch();

  //cambio tutti gli usi della variabile induttiva del secondo loop con la variabile induttiva del primo loop
  PHINode* loop1var = dyn_cast<PHINode>(header_prev->begin());
  PHINode* loop2var = dyn_cast<PHINode>(header_actual->begin());
  loop2var->replaceAllUsesWith(loop1var);
  loop2var->eraseFromParent(); //una volta che ho sostituito la seconda variabile induttiva, procedo anche a cancellarla

  //collego il body del loop predecedente al body del loop attuale
  BranchInst* body_prev_bi = dyn_cast<BranchInst>(body_prev->getTerminator());
  body_prev_bi->setSuccessor(0,body_actual);

  //collego l'header del loop precedente all'exiting del loop attuale
  BranchInst *header_prev_bi = dyn_cast<BranchInst>(header_prev->getTerminator());
  header_prev_bi->setSuccessor(1,exiting_actual);

  //collego body del loop attuale al latch del loop precedente
  BranchInst* body_actual_bi = dyn_cast<BranchInst>(body_actual->getTerminator());
  body_actual_bi->setSuccessor(0,latch_prev);
}

bool checkAdiacenza(Loop* l_actual, Loop* l_prev) {
  return l_prev->getExitBlock() == l_actual->getLoopPreheader();
}

bool checkBranch(Loop* l_actual) {
  BasicBlock* preheader = l_actual->getLoopPreheader();
  Instruction *inst = dyn_cast<Instruction>(preheader->begin());
  if (llvm::BranchInst* branchInst = dyn_cast<BranchInst>(inst))
    return true;
  else
    return false;
}

bool checkCFEquivalent(Loop* l_actual, Loop* l_prev, DominatorTree& DT, PostDominatorTree& PDT) {
    return DT.dominates(l_prev->getHeader(),l_actual->getHeader()) && PDT.dominates(l_actual->getHeader(),l_prev->getHeader());
}

bool checkLoopTripCount(Loop* attuale, Loop* prev, ScalarEvolution &SE) {
  return SE.getSmallConstantTripCount(attuale) == SE.getSmallConstantTripCount(prev);
}