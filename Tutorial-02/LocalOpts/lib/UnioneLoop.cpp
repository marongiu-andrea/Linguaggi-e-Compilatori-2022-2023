#include "LocalOpts.h"
/*
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
*/


using namespace llvm;


PreservedAnalyses UnioneLoopPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &AM) {

  /*// Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunctionMultiInstr(*Iter)) {
      return PreservedAnalyses::none();
    }
  }
    */
  return PreservedAnalyses::none();
}

