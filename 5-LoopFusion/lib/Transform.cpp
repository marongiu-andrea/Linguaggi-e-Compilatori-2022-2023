#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>
using namespace llvm;

bool runOnBasicBlock(BasicBlock &BB)
{
  int BID = 0;
  outs() << "Basic Block: " << BB.getName() << "\n";
  for (auto &I : BB)
  {
    outs() << "Instruction: " << I << "\n";
    outs() << "Instruction ID: " << BID << "\n";
    outs() << "Instruction Opcode: " << I.getOpcode() << "\n";
  }
  return true;
}

bool runOnFunction(Function &F)
{
  bool Transformed = false;

  for (auto &BB : F)
  {
    if (runOnBasicBlock(BB))
    {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F,
                                     FunctionAnalysisManager &AM)
{
  auto &LI = AM.getResult<LoopAnalysis>(F);
  int l = 0;

  runOnFunction(F);

  // Un semplice passo di esempio di manipolazione della IR
  // for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
  // {
  //   if (runOnFunction(*Iter))
  //   {
  //     return PreservedAnalyses::none();
  //   }
  // }

  return PreservedAnalyses::none();
}
