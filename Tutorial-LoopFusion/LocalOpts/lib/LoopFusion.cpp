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

  for(auto &use : InductionL2->uses()){
    BasicBlock *parentBlock = dyn_cast<Instruction>(use)->getParent();
    if(parentBlock != L2->getLoopPreheader() && parentBlock != L2->getLoopLatch()){
      User *user = use.getUser();
      user->replaceUsesOfWith(use, InductionL1);
    }
  }

  BasicBlock *L2_Exit = L2->getExitingBlock();
  BasicBlock *L2_BodyStart = L2->getHeader()->getNextNode();
  BasicBlock *L2_Latch = L2->getLoopLatch();
  BasicBlock *L1_Latch = L1->getLoopLatch();
  BasicBlock *L1_Header = L1->getHeader();
  BasicBlock *L2_Header = L2->getHeader();

  BasicBlock *L2_BodyEnd;
  for(auto *BB : L2->getBlocks()){
    if(BB == L2->getLoopLatch())
      break;
    L2_BodyEnd = BB;
  }

  L1_Header->getTerminator()->replaceSuccessorWith(L2->getLoopPreheader(), L2_Exit);
  L1_Header->getNextNode()->getTerminator()->replaceSuccessorWith(L1_Latch, L2_BodyStart);
  L2_BodyEnd->getTerminator()->replaceSuccessorWith(L2_Latch, L1_Latch);
  L2_Header->getTerminator()->replaceSuccessorWith(L2_BodyStart, L2_Latch);



/*
  // Ottenere i blocchi del primo loop
  SmallVector<BasicBlock*> L1Blocks;
  for (auto* BB : L1->getBlocks())
    L1Blocks.push_back(BB);

  // Ottenere i blocchi del secondo loop
  SmallVector<BasicBlock*> L2Blocks;
  for (auto* BB : L2->getBlocks())
    L2Blocks.push_back(BB);

  // Trovare il blocco di terminazione del primo loop
  BasicBlock* L1ExitBlock = L1->getExitBlock();

  // Trovare il blocco di test del secondo loop
  BasicBlock* L2HeaderBlock = L2->getHeader();

  // Collegare il blocco di terminazione del primo loop al blocco di test del secondo loop
  L1ExitBlock->getTerminator()->replaceSuccessorWith(L2HeaderBlock, L1->getExitingBlock());

  //La logica del CFG e' sbagliata: l'header di L1 deve puntare L2_Exit, e l'header di L2 deve puntare al latch di L2

  // Aggiornare il CFG
  LI.removeBlock(L2HeaderBlock);
  LI.changeLoopFor(L1ExitBlock, L1);
  L1->addBasicBlockToLoop(L2HeaderBlock, LI);
  L1Blocks.insert(L1Blocks.end(), L2Blocks.begin(), L2Blocks.end());

  // Rimuovere il secondo loop dal LoopInfo
  LI.erase(L2);
*/
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

