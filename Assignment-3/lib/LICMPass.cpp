#include "LICMPass.h"
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <vector>

using namespace llvm;

char LICMPass::ID = 0;
RegisterPass<LICMPass> X("loop-invariant-code-motion","Loop Invariant Code Motion");

/*
  [Algoritmo per la Code Motion]
  1. Calcolare le reaching definitions
  2. Trovare le istruzioni loop-invariant
  3. Calcolare i dominatori (dominance tree)
  4. Trovare le uscite del loop (i successori fuori dal loop)
  5. Le istruzioni candidate alla code motion:
    5.1 Sono loop invariant
    5.2 Si trovano in blocchi che dominano tutte le uscite del loop
    5.3 Le variabili sono assegnate una solo volta nel loop
    5.4 Si trovano in blocchi che dominano tutti i blocchi nel loop che usano la
        variabile a cui si sta assegnando un valore
  6. Eseguire una ricerca depth-first dei blocchi
    6.1 Spostare l’istruzione candidata nel preheader
*/


bool LICMPass::isIntructionLoopInvariant( Instruction* inst,  Loop* L)
{
  Value* op1 = inst->getOperand(0);
  Value* op2 = inst->getOperand(1);
  return isOperandLoopInvariant(op1, L) && isOperandLoopInvariant(op2, L);
}

bool LICMPass::isOperandLoopInvariant( Value* op, Loop* L)
{
  // l'operando è una costante?
  if(isa<ConstantInt>(op))
    return true;

  // l'operando è definito fuori dal loop?
  Instruction* instrOP = dyn_cast<Instruction>(op);
  if(instrOP && !L->contains(instrOP))
    return true;  
  
  // l'operando è stato marcato come loop-invariant?
  if(markedAsLoopInvariants.find(instrOP) != markedAsLoopInvariants.end())
    return true;

  return false;
}

bool LICMPass::isExitBasicBlock(BasicBlock* BB,Loop* L)
{
  // è un exit basic block se il suo next è fuori dal loop
  return !L->contains(BB->getNextNode());
}

bool LICMPass::dominatesAllExitBlocks(Instruction* instr, DominatorTree* dt)
{
  for(auto bb : exitBasicBlocks)
    if(!dt->dominates(instr, bb))
      return false;
  
  return true;
}

bool LICMPass::isDeadOutOfLoop(Instruction* instr, Loop* L)
{
  // l'istruzione viene usata solo all'interno del loop
  for(const auto& use : instr->uses())
  {
    Instruction* user = dyn_cast<Instruction>(use.getUser());
    if(!L->contains(user))
      return false;
  }

  return true;
}



void LICMPass::getAnalysisUsage(AnalysisUsage &AU) const 
{ 
  AU.setPreservesAll();
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<LoopInfoWrapperPass>();
}

bool LICMPass::runOnLoop(Loop* L, LPPassManager& LPM)
{
  auto& os = outs();

  if(!L->isLoopSimplifyForm()) return false;
  
  DominatorTree& dominatorTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  
  os << "------ Iterate over Loop ------\n";

  // cerco tutti gli exit blocks del loop
  // cerco tutte le instruzioni loop-invariant
  for(auto BB : L->getBlocks())
  {
    bool isExitBlock = isExitBasicBlock(BB, L); 
    os << *BB << "isExitBasicBlock=" << (isExitBlock == 0 ? "False" : "True") << "\n";
    if(isExitBlock)
      exitBasicBlocks.insert(BB);
    
    for(auto& instr : BB->getInstList())
    {
      // considero solo instruzioni del tipo A = B + C 
      if(!instr.isBinaryOp()) continue;
      
      bool isLoopInvariant = isIntructionLoopInvariant(&instr, L); 
      os << "<" << instr << ", " << isLoopInvariant << ">\n";
      
      if(isLoopInvariant)
        markedAsLoopInvariants.insert(&instr);
    }
    os << "\n\n";
  }

  // cerco le istruzioni candidate tra quelle marcate loop-invariant
  for(auto inst : markedAsLoopInvariants)
  {
    if(dominatesAllExitBlocks(inst, &dominatorTree) || isDeadOutOfLoop(inst, L))
      hoistableInstructions.insert(inst);
  }


  os << "\n--------------------\n";
  os << "Loop invariant instructions:\n";
  for(const auto inst : markedAsLoopInvariants)
    os << inst << "\n";
  os << "\n";

  os << "Hoistable instructions:\n";
  for(const auto inst : hoistableInstructions)
    os << inst << "\n";
  os << "\n";

  

  return false; 
}



