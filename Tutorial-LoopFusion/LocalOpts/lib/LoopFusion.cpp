#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/PostDominators.h"
#include <llvm/ADT/SetVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Casting.h>

using namespace llvm;

bool areBlocksAdjacent(const llvm::BasicBlock* block1, const llvm::BasicBlock* block2) {
    const Instruction* terminator = block1->getTerminator();
      // Verifica se il ramo non condizionale punta al secondo basic block
      if (terminator->getSuccessor(0) == block2) {
          return true;
      }
    return false;
}

bool isLoopControlFlowEquivalent(Loop *L1, Loop *L2, DominatorTree &DT, PostDominatorTree &PDT) {
  // Check se L1 domina L2
  if (!DT.dominates(L1->getLoopPreheader(), L2->getLoopPreheader()))
    return false;

  // Check se L2 post-domina L1
  if (!PDT.dominates(L2->getLoopPreheader(), L1->getLoopPreheader()))
    return false;

  return true;
}

void mergeLoops(Loop* L1, Loop* L2, LoopInfo& LI) {

  //Operiamo sulle Induction variables
  PHINode *InductionL1 = L1->getCanonicalInductionVariable();
  PHINode *InductionL2 = L2->getCanonicalInductionVariable();

  // for(auto &use : InductionL2->uses()){
  //    BasicBlock *parentBlock = dyn_cast<Instruction>(use)->getParent();
  //    if(parentBlock != L2->getLoopPreheader() && parentBlock != L2->getLoopLatch()){
  //       User *user = use.getUser();
  //       user->replaceUsesOfWith(use, InductionL1);
  //    }
  // }

  InductionL2->replaceAllUsesWith(InductionL1);
  InductionL2->eraseFromParent();

  BasicBlock *L1_Latch = L1->getLoopLatch();
  BasicBlock *L1_Header = L1->getHeader();
  BasicBlock *L1_Body = L1_Header->getTerminator()->getSuccessor(0);

  BasicBlock *L2_Header = L2->getHeader();
  BasicBlock *L2_Latch = L2->getLoopLatch();
  BasicBlock *L2_Exit = L2->getExitBlock();
  BasicBlock *L2_Body = L2_Header->getTerminator()->getSuccessor(0);

  L1_Header->getTerminator()->setSuccessor(1, L2_Exit);
  L1_Body->getTerminator()->setSuccessor(0, L2_Body);
  L2_Body->getTerminator()->setSuccessor(0, L1_Latch);
  L2_Header->getTerminator()->setSuccessor(0, L2_Latch);

}

PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &AM) {

  //Analisi necessarie
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  
  int l = 0;
  SmallVector<Loop*> PreOrderLoops = LI.getLoopsInPreorder();
  Loop *LP = nullptr;

  for(Loop *L: PreOrderLoops){
    outs() << "PROCESSING LOOP: " << ++l << "\n";
    if(LP == nullptr){
      LP = L;
      continue;
    }

    //Se i due loop non sono adiacenti continua
    if(!areBlocksAdjacent(LP->getExitBlock(),L->getHeader())){
      LP = L;
      continue;
    }

    outs() << "Loop Adiacente trovato\n";

    //Crea i count delle iterazioni dei loop
    int TripCountL = SE.getSmallConstantTripCount(L);
    int TripCountLP = SE.getSmallConstantTripCount(LP);


    //Se le iterazioni dei loop non sono uguali continua
    if(TripCountLP != TripCountL){
      LP = L;
      continue;
    }
    outs() << "Numero di iterazioni uguale\n";

    //Se I due loop non sono CFG equivalenti continua
    if(!isLoopControlFlowEquivalent(LP,L,DT,PDT)){
      LP = L;
      continue;
    }
    outs() << "I due loop sono Control Flow Equivalent\n";

//  Per il punto di Negative Distance Dependencies, Abbiamo considerato sia implicito nel punto 3, dato che il loop1 domina il loop2

    mergeLoops(LP, L, LI);

    outs() << "Codice sostituito\n";

    //Avanziamo LP
    LP = L;
  }

  return PreservedAnalyses::none();
}

