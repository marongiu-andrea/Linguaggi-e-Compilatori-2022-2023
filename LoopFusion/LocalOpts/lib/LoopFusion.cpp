#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;

//Per generare il file .ll da Loop.c
//clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.c
//opt -passes=mem2reg Loop.bc -o Loop.opt.bc
//llvm-dis Loop.opt.bc -o Loop.opt.ll


PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &FAM) 
{ 	
	auto &LI = FAM.getResult<LoopAnalysis>(F);

	//1) The loops must be adjacent (there cannot be any statements between the two loops).

	//SmallVector<LoopT *, 4> loops = LI.getLoopsInPreorder();


	//2) The loops must be conforming (they must execute the same number of iterations).

	//3) The loops must be control flow equivalent (if one loop executes, the other is guaranteed to execute).
	
	//4) There cannot be any negative distance dependencies between the loops. If all of these conditions are satisfied, it is safe to fuse the loops.

	return PreservedAnalyses::none();
}

