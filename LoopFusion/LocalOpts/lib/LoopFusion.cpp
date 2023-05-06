#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Frontend/OpenMP/OMPIRBuilder.h"

using namespace llvm;

//Per generare il file .ll da Loop.c
//clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.c
//opt -passes=mem2reg Loop.bc -o Loop.opt.bc
//llvm-dis Loop.opt.bc -o Loop.opt.ll

//opt -load-pass-plugin=./libLocalOpts.so -passes=loopfusion test/Loop.opt.ll -o test/test.loopfusion.optimized.bc
//llvm-dis test/test.loopfusion.optimized.bc -o test/test.loopfusion.optimized.ll

llvm::PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &FAM) 
{
	auto &LI = FAM.getResult<LoopAnalysis>(F);

	SmallVector<Loop *> loops = LI.getLoopsInPreorder(); // SmallVector contenente tutti i loop
	SmallVector<BasicBlock *> loopExits;
	
	bool adjacent; // True - Se tutte le uscite del primo loop sono uguali al preheader del secondo

	// Itera su tutti i loop
	for (auto &IterLoop1 : loops)
	{
		for (auto &IterLoop2 : loops)
		{
			if (IterLoop1 != IterLoop2)
			{
				adjacent = true;

				(*IterLoop1).getExitBlocks(loopExits);
				BasicBlock * preheaderL2 = (*IterLoop2).getLoopPreheader();

				// Itera su tutte le uscite di IterLoop1
				for (auto &IterExit : loopExits)
				{
					// Affinché due loop siano adiacenti è necessario assicurarsi che tutte 
					//  le uscite di IterLoop1 corrispondano al preheader di IterLoop2
					if (IterExit != preheaderL2)
						adjacent = false;
				}	
				if (adjacent) 
				{
					outs()<<"I Loop:\n"<<*IterLoop1<<*IterLoop2<<" sono adiacenti\n----------------------\n";

					// Assumiamo per semplicità che la condizione di controllo del loop sia 
					//  solo una, il controllo venga effettuato per una variabile nel LHS
					//   per una costante nel RHS


				}
			}

		}
	}


	//2) The loops must be conforming (they must execute the same number of iterations).

	//3) The loops must be control flow equivalent (if one loop executes, the other is guaranteed to execute).
	
	//4) There cannot be any negative distance dependencies between the loops. If all of these conditions are satisfied, it is safe to fuse the loops.
  	return PreservedAnalyses::none();
}

