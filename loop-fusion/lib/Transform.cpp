#include "LocalOpts.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
using namespace llvm;

/*
  [Condizioni per la loop fusion]
  Dati due loop Li e Lj, con Li che precede Lj, possiamo applicare la tecnica
  del loop fusion se Li e Lj:
  1. sono adiacenti (non ci sono blocchi intermedi che separano i due loop)
  2. hanno stesso numero di iterazioni (Loop Trip Count)
  3. hanno entrambi un singolo entry point e un singolo exit point(stesso control flow);
    se hanno lo statement break non posso eseguire la loop fusion
  4. non devono avere dipendeze a distanza negativa:
    ovvero quando all'iterazione m nel loop Lj viene usato un valore 
    che viene calcolato all'iterazione m+n in Li

  Una volta verificato tutte le condizioni:
  1. modificare gli usi della induction variable nel body del loop 2 con 
    quelli della induction variable del loop 1
  2. modificare il CFG perché il body del loop 2 sia
    agganciato a seguito del body del loop 1 nel loop 1
*/

bool LoopFusionPass::areAdjacent(Loop* Li, Loop* Lj)
{
  return Li->getExitBlock() == Lj->getLoopPreheader();
}

bool LoopFusionPass::sameTripCount(Loop* Li, Loop* Lj, ScalarEvolution& SE)
{
  return SE.getSmallConstantTripCount(Li) == SE.getSmallConstantTripCount(Lj) &&
    SE.getSmallConstantMaxTripCount(Li) == SE.getSmallConstantMaxTripCount(Lj);
}

bool LoopFusionPass::sameGuardBranch(Loop* Li, Loop* Lj)
{
  return Li->getLoopGuardBranch() == Lj->getLoopGuardBranch();
}

bool LoopFusionPass::controlFlowEquivalence(Loop* Li, Loop* Lj, DominatorTree& dt, PostDominatorTree& pdt)
{
  return dt.dominates(Li->getHeader(), Lj->getHeader()) && 
    pdt.dominates(Lj->getHeader(), Li->getHeader());  
}

// TODO
bool LoopFusionPass::haveNegativeDistanceDependence(Loop*, Loop*)
{
  return true; 
}

void LoopFusionPass::mergeLoops(Loop* Li, Loop* Lj, ScalarEvolution& SE)
{
  auto& os = outs();

  auto Li_Header = Li->getHeader();
  auto Li_Body   = (dyn_cast<BranchInst>(Li_Header->getTerminator()))->getSuccessor(0);
  auto Li_Latch  = Li->getLoopLatch();

  auto Lj_Header = Lj->getHeader();
  auto Lj_Body   = (dyn_cast<BranchInst>(Lj_Header->getTerminator()))->getSuccessor(0);
  auto Lj_Exiting= Lj->getExitingBlock();

  /*
  1.Modificare gli usi della induction variable nel body di Lj
    con quelli della induction variable di Li
  */

  auto inductionVar_i = dyn_cast<PHINode>(Li_Header->begin()); 
  auto inductionVar_j = dyn_cast<PHINode>(Lj_Header->begin()); 
  inductionVar_j->replaceAllUsesWith(inductionVar_i);
  inductionVar_j->eraseFromParent();

  //collego l'header del loop Li all'exiting del loop Lj
  auto branchInstruction = dyn_cast<BranchInst>(Li_Header->getTerminator());
  branchInstruction->setSuccessor(1, Lj_Exiting);

  
  // collego il body del loop Li al body del loop Lj
  branchInstruction = dyn_cast<BranchInst>(Li_Body->getTerminator());
  branchInstruction->setSuccessor(0, Lj_Body);
  
  //collego body del loop Lj al latch del loop Li
  branchInstruction = dyn_cast<BranchInst>(Lj_Body->getTerminator());
  branchInstruction->setSuccessor(0,Li_Latch);
}

PreservedAnalyses LoopFusionPass::run(Function& F, FunctionAnalysisManager& FAM) 
{
  auto& os = outs();
  os << "----- START LOOP FUSION PASS -----\n";

  auto& loopInfo      = FAM.getResult<LoopAnalysis>(F);
  auto& dominatorTree = FAM.getResult<DominatorTreeAnalysis>(F);
  auto& postDT        = FAM.getResult<PostDominatorTreeAnalysis>(F);
  auto& SE            = FAM.getResult<ScalarEvolutionAnalysis>(F);

  auto loops = loopInfo.getLoopsInPreorder();

  Loop* Li = nullptr;
  Loop* Lj = nullptr;
  for(auto loopIt = loops.begin(); loopIt != loops.end(); ++loopIt)
  {
    Li = *loopIt;
    Lj = *(++loopIt);

    if(!Li || !Lj)
      break;
    
    if(!areAdjacent(Li, Lj))
    {
      os << "Li, Lj are not adjacent!\n";
      continue;
    }
    if(!sameTripCount(Li, Lj, SE))
    {
      os << "Li, Lj have different Trip count!\n";
      continue;
    }
    if(!sameGuardBranch(Li, Lj))
    {
      os << "Li, Lj non hanno lo stesso Guard Branch!\n";
      continue;
    }
    if(!controlFlowEquivalence(Li, Lj, dominatorTree, postDT))
    {
      os << "Li, Lj have different control flow!\n";
      continue;
    }

    os << "Li, Lj satisfy the conditions! Merge them...\n";

    mergeLoops(Li, Lj, SE);
  }

  os << "----- END LOOP FUSION PASS -----\n";

  return PreservedAnalyses::none();
}

