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
      if(checkAdiacenza(l_actual, l_prev)){ //ho verificato che il preheader del scondo loop coincide con l'exit del primo loop
        if(checkBranch(l_actual)) { //verifico che il preheader del secondo loop abbia come prima istruzione una branch
          if(SE.getSmallConstantTripCount(l_actual) == SE.getSmallConstantTripCount(l_prev)){
            /*
            std::optional<Loop::LoopBounds> *lb_prev = dyn_cast<std::optional<Loop::LoopBounds>>(l_prev->getBounds(SE));
            if (lb_prev == std::nullopt) {
              outs() << "E' null!";
            }
            else {
              outs() << "Non e' null!\n";
            }
            */
           outs() << "Faccio la loop fusion dioporcone!!\n";
           fondiLoop(l_actual, l_prev,SE);
          }
          
        }
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
  outs() << "Sono pronto a cambiare gli usi della induction variable!\n";
  PHINode* loop1var = dyn_cast<PHINode>(header_prev->begin());
  outs() << "ho preso la induction variable!\n";
  loop1var->print(outs());
  outs() << "\n";
  PHINode* loop2var = dyn_cast<PHINode>(header_actual->begin());
  outs() << "Ho preso la seconda induction variable!\n";
  loop2var->print(outs());
  outs() << "\n";
  loop2var->replaceAllUsesWith(loop1var);
  loop2var->eraseFromParent();
  outs() << "Ho fatto tutto mannaggia a te\n";

  //collego il body del loop predecedente al body del loop attuale
  BranchInst* body_prev_bi = dyn_cast<BranchInst>(body_prev->getTerminator());
  body_prev_bi->setSuccessor(0,body_actual);
  body_prev_bi->print(outs());
  outs() << "\n";

  //collego l'header del loop precedente all'exiting del loop attuale
  BranchInst *header_prev_bi = dyn_cast<BranchInst>(header_prev->getTerminator());
  header_prev_bi->setSuccessor(1,exiting_actual);
  header_prev_bi->print(outs());
  outs() << "\n";

  //collego body del loop attuale al latch del loop precedente
  BranchInst* body_actual_bi = dyn_cast<BranchInst>(body_actual->getTerminator());
  body_actual_bi->setSuccessor(0,latch_prev);
  body_actual_bi->print(outs());
  outs() << "\n";
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

}