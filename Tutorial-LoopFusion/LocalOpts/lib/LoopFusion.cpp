#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/PostDominators.h"
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
  if (!DT.dominates(L1->getHeader(), L2->getHeader()))
    return false;

  // Check se L2 post-domina L1
  if (!PDT.dominates(L2->getHeader(), L1->getHeader()))
    return false;

  return true;
}

void mergeLoops(Loop* L1, Loop* L2, LoopInfo& LI) {

  //Operiamo sulle Induction variables
  PHINode *InductionL1 = L1->getCanonicalInductionVariable();
  PHINode *InductionL2 = L2->getCanonicalInductionVariable();

  InductionL2->replaceAllUsesWith(InductionL1);

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

  // Aggiornare il CFG
  LI.removeBlock(L2HeaderBlock);
  LI.changeLoopFor(L1ExitBlock, L1);
  L1->addBasicBlockToLoop(L2HeaderBlock, LI);
  L1Blocks.insert(L1Blocks.end(), L2Blocks.begin(), L2Blocks.end());

  // Rimuovere il secondo loop dal LoopInfo
  LI.erase(L2);
}

PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &AM) {

  //Analisi necessarie
  LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  
  int l = 0;
  int BID = 0;
  SmallVector<Loop*> PreOrderLoops = LI.getLoopsInPreorder();
  Loop *LP = nullptr;

  for(Loop *L: PreOrderLoops){
    outs() << "PROCESSING LOOP: " << ++l << "\n";
    if(LP == nullptr)
    {
      LP = L;
      continue;
    }

    //Se i due loop non sono adiacenti continua
    if(!areBlocksAdjacent(LP->getExitBlock(),L->getHeader()))
      continue;

    outs() << "Loop Adiacente trovato\n";

    //Crea i count delle iterazioni dei loop
    const SCEV *TripCountSCEVLP = SE.getBackedgeTakenCount(LP);
    const APInt TripCountLP = dyn_cast<SCEVConstant>(TripCountSCEVLP)->getAPInt();
    const SCEV *TripCountSCEVL = SE.getBackedgeTakenCount(L);
    const APInt TripCountL = dyn_cast<SCEVConstant>(TripCountSCEVL)->getAPInt();

    //Se le iterazioni dei loop non sono uguali continua
    if(!TripCountLP == TripCountL)
        continue;
    outs() << "Numero di iterazioni uguale\n";

    //Se I due loop non sono CFG equivalenti continua
    if(!isLoopControlFlowEquivalent(LP,L,DT,PDT))
      continue;
    outs() << "I due loop sono Control Flow Equivalent\n";

    mergeLoops(LP, L, LI);

    outs() << "Codice sostituito\n";

    //Avanziamo LP
    LP = L;
  }

  return PreservedAnalyses::none();
}

