#include "LICMPass.h"
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

char LICMPass::ID = 0;
RegisterPass<LICMPass> X("loop-invariant-code-motion","Loop Invariant Code Motion");

void LICMPass::getAnalysisUsage(AnalysisUsage &AU) const 
{ 


  
  AU.setPreservesAll();
}

bool LICMPass::runOnLoop(Loop* L, LPPassManager& LPM)
{
  auto& os = outs();

  

  os << "\n BEGINNING LOOP INVARIANT CODE MOTION PASS...\n"; 
  return false; 
}



