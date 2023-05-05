#include "LocalOpts.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <string.h>
using namespace llvm;


bool runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F,FunctionAnalysisManager &AM) {
  auto &LI = AM.getResult<LoopAnalysis>(F);
  int l = 0;
  int BID = 0;
  SmallVector<Loop *, 4> PreOrderLoops = LI.getLoopsInPreorder();
  for(Loop *L : PreOrderLoops){
    outs()<<"\n***************PROCESSING LOOP***************";
    BasicBlock *PH = L->getLoopPreheader();
    if(PH) outs()<<" "<<*PH<<"\n";
  }
                                
 
  }