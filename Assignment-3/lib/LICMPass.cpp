#include "LICMPass.h"
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>


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

void LICMPass::getLoopInvariantInstructions()
{
  for(auto& BB : loop->getBlocks())
  {    
    for(auto& instr : BB->getInstList())
    {
      // cerco le istruzioni del tipo A = B + C
      if(!instr.isBinaryOp()) continue;

      if(isIntructionLoopInvariant(&instr))
        markedAsLoopInvariants.insert(&instr);
    }
  }
}

bool LICMPass::isIntructionLoopInvariant(Instruction* inst)
{
  // un'istruzione è loop-invariant se ha tutti gli operandi loop-invariant
  for(auto& op : inst->operands())
    if(!isOperandLoopInvariant(op.get()))
      return false;
  
  return true;
}

bool LICMPass::isOperandLoopInvariant(Value* op)
{
  // l'operando è una costante o argomento di una funzione
  if(isa<Constant>(op) || isa<Argument>(op))
    return true;
  
  // l'operando è una variabile
  Instruction* definition = dyn_cast<Instruction>(op);
  if(definition)
  {
    // l'operando è una variabile ed è definita solo fuori dal loop?
    if(!loop->contains(definition))
      return true;
    
    // l'operando è una variabile definita dentro al loop, 
    // se non è un PHI node ho una unica reaching definition dentro al loop, 
    // se è già marcato come loop-invariant
    if(loop->contains(definition) && 
        !isa<PHINode>(definition) && 
        markedAsLoopInvariants.count(definition) != 0)
      return true;
  }

  return false;
}


void LICMPass::getHoistableInstructions()
{
  for(auto inst : markedAsLoopInvariants)
    if(dominatesAllExitingBlocks(inst) || isDeadOutOfLoop(inst))
      hoistableInstructions.insert(inst); 
}

bool LICMPass::dominatesAllExitingBlocks(Instruction* instr)
{
  for(auto bb : exitingBasicBlocks)
    if(!dominatorTree->dominates(instr, bb))
      return false;
  
  return true;
}

bool LICMPass::isDeadOutOfLoop(Instruction* instr)
{
  // l'istruzione viene usata solo all'interno del loop
  for(const auto& use : instr->uses())
  {
    Instruction* user = dyn_cast<Instruction>(use.getUser());
    if(!loop->contains(user))
      return false;
  }

  return true;
}


void LICMPass::moveInstructions()
{
  const auto header     = loop->getHeader();
  const auto preHeader  = loop->getLoopPreheader();
  const auto terminator = preHeader->getTerminator();

  for(auto inst : hoistableInstructions)
    inst->moveBefore(terminator);
}


void LICMPass::printLoopInvariantInstructions()
{
  auto& os = outs();
  os << "\n----- Print loop invariant instructions -----\n";
  for(const auto& inst : markedAsLoopInvariants)
    os << *inst << "\n";
  os << "----------------------------------\n";
}

void LICMPass::printHoistableInstructions()
{
    auto& os = outs();
  os << "\n----- Print loop hoistble instructions -----\n";
  for(const auto& inst : hoistableInstructions)
    os << *inst << "\n";
  os << "----------------------------------\n";
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
  
  loop = L;
  dominatorTree = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  
  // cerco tutti gli exiting blocks del loop
  loop->getExitingBlocks(exitingBasicBlocks);
  
  // cerco tutte le istruzioni loop-invariant
  getLoopInvariantInstructions();
  // cerco le istruzioni candidate tra quelle marcate loop-invariant
  getHoistableInstructions();

  printLoopInvariantInstructions();
  printHoistableInstructions();

  // sposto le istruzioni candidate nel preheader
  moveInstructions();

  return true; 
}
