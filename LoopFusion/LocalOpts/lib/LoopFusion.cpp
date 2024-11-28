#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

using namespace llvm;

// -----------------------------------------------------------------------------------
// Per generare il file .ll da Loop.c
//  clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.c
//  opt -passes=mem2reg Loop.bc -o Loop.opt.bc
//  llvm-dis Loop.opt.bc -o Loop.opt.ll

// mem2reg - The mem2reg pass converts non-SSA form of LLVM IR into SSA form,
//  raising loads and stores to stack-allocated values to “registers” (SSA values).
// -----------------------------------------------------------------------------------

// opt -load-pass-plugin=./libLocalOpts.so -passes=loopfusion test/Loop.opt.ll -o test/test.loopfusion.optimized.bc
// llvm-dis test/test.loopfusion.optimized.bc -o test/test.loopfusion.optimized.ll
// -----------------------------------------------------------------------------------

// Ritorna true se i due loop analizzati sono adiacenti.
//  Due Loop sono adiacenti solo quando l'exit block del primo loop corrisponde al preheader del secondo loop
//   e l'exit block del primo Loop contiene solo un'istruzione di branch che porta all'header del secondo loop
bool LoopFusionPass::areLoopsAdjacent(BasicBlock * exitBlockL1, BasicBlock * preheaderL2, BasicBlock * headerL2)
{
	// Se l'exit Block del Loop1 coincide con il preheader del Loop2
	if (exitBlockL1 == preheaderL2)
	{
		// Se la PRIMA istruzione dell'exit Block del Loop1 è una branch, e se la branch porta al Loop2
		if (((*exitBlockL1).front()).getOpcode() == Instruction::Br 
			&& ((*exitBlockL1).front()).getOperand(0) == headerL2)
		{
			outs()<<"La prima istruzione dell'exit block del loop1 è una branch che porta al Loop2\n";
			return true;
		}
	}
	outs()<<"I due loop NON sono adiacenti\n";
	return false;
}

// Ritorna true se i trip count dei due loop sono uguali, false se sono diversi o non calcolabili
bool LoopFusionPass::sameTripCount(Loop * L1, Loop * L2, ScalarEvolution &SE)
{
	const SCEV * tripCountL1 = SE.getBackedgeTakenCount(L1);
	const SCEV * tripCountL2 = SE.getBackedgeTakenCount(L2);

	outs()<<"TRIP COUNT L1: "<<*tripCountL1<<"\n";
	outs()<<"TRIP COUNT L2: "<<*tripCountL2<<"\n";

	// Controlla che il trip count tra i due loop sia calcolabile e che sia uguale
	if ((!isa<SCEVCouldNotCompute>(tripCountL1)) && (!isa<SCEVCouldNotCompute>(tripCountL2)) && (tripCountL1 == tripCountL2))
		return true;

	return false;
}

// Due Loop sono Control-Flow Equivalent se l'esecuzione di uno assicura l'esecuzione dell'altro.
//  Per verificare che due loop siano Control-Flow Equivalent si usa l'analisi dei dominatori e
//   dei post-dominatori:
//    Se il Loop j domina il Loop k, e il Loop k post-domina il loop j
//     ---> i due Loop sono Control-Flow Equivalent

// Dominator: un nodo x domina un nodo y se ogni percorso dall'entry block a y contiene x
// Post-dominator: un nodo y post-domina un nodo x se ogni percorso da x all'uscita contiene y
// ----------------------------------------------------------------------------------------------
// Ritorna true se i loop sono Control-Flow Equicalent, false altrimenti
bool LoopFusionPass::areLoopsControlFlowEquivalent(Loop * L1, Loop * L2, DominatorTree * DT, PostDominatorTree *PDT)
{
	// Se IterLoop1 domina IterLoop2 e se IterLoop2 post-domina IterLoop1 sono Control-Flow Equivalent
	if ((*DT).dominates((*L1).getLoopPreheader(), (*L2).getLoopPreheader()) 
		&& (*PDT).dominates((*L2).getLoopPreheader(), (*L1).getLoopPreheader())) 
		return true;

	return false;
}

// Esegue la loop fusion:
void LoopFusionPass::loopFusion(Loop * L1, Loop * L2)
{
	// Loop 1
	BasicBlock * latchL1 = (*L1).getLoopLatch();
	BasicBlock * endBodyL1 = (*latchL1).getPrevNode();
	Instruction * bodyTerminatorL1 = (*endBodyL1).getTerminator();
	BasicBlock * headerL1 = (*L1).getHeader();
	Instruction * headerTerminatorL1 = (*headerL1).getTerminator();
	Instruction * headerPhiL1 = dyn_cast<Instruction>((*headerL1).begin());

	// Loop 2
	BasicBlock * headerL2 = (*L2).getHeader();
	Instruction * headerTerminatorL2 = (*headerL2).getTerminator();
	BasicBlock * latchL2 = (*L2).getLoopLatch();
	BasicBlock * beginBodyL2 = (*headerL2).getNextNode();
	BasicBlock * endBodyL2 = (*latchL2).getPrevNode();
	Instruction * bodyTerminatorL2 = (*endBodyL2).getTerminator();
	BasicBlock * exitBlockL2 = (*L2).getExitBlock();
	Instruction * headerPhiL2 = dyn_cast<Instruction>((*headerL2).begin());

	// 1) Connette il Body del Loop 1 con il Body del Loop 2
	(*bodyTerminatorL1).setSuccessor(0, beginBodyL2);

	// 2) Rimpiazza tutti gli usi della istruzione PHI dell'header del Loop 2 con la PHI dell'header del Loop 1
	(*headerPhiL2).replaceAllUsesWith(headerPhiL1);
	
	// 3) Connette il Body del Loop 2 con il Latch del Loop 1
	(*bodyTerminatorL2).setSuccessor(0, latchL1);

	// 4) L'Exit Block del Loop 1 diventa l'Exit Block del Loop 2
	(*headerTerminatorL1).setSuccessor(1, exitBlockL2);

	// 5) L'header del Loop 2 viene connesso al Latch del loop 2
	(*headerTerminatorL2).setSuccessor(0, latchL2);
}


llvm::PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Function &F, FunctionAnalysisManager &FAM) 
{
	auto &LI = FAM.getResult<LoopAnalysis>(F);
	ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
	DominatorTree * DT = &FAM.getResult<DominatorTreeAnalysis>(F);
	PostDominatorTree *PDT = &FAM.getResult<PostDominatorTreeAnalysis>(F);

	SmallVector<Loop *> loops = LI.getLoopsInPreorder(); // SmallVector contenente tutti i loop
	
	int loop1 = 0;
	int loop2 = 0;

	// Itera su tutti i loop
	for (auto &IterLoop1 : loops)
	{
		loop1++;
		loop2 = 0;
		for (auto &IterLoop2 : loops)
		{
			loop2++;
			if (IterLoop1 != IterLoop2)
			{
				outs()<<"--> Analizzo il loop "<<loop1<<" e il loop "<<loop2<<":\n";

				BasicBlock * exitBlockL1 = (*IterLoop1).getExitBlock();
				BasicBlock * preheaderL2 = (*IterLoop2).getLoopPreheader();
				BasicBlock * headerL2 = (*IterLoop2).getHeader();

				if (areLoopsAdjacent(exitBlockL1, preheaderL2, headerL2))
				{
					outs()<<"I Loop:\n"<<*IterLoop1<<*IterLoop2<<" sono adiacenti\n----------------------\n";

					if (sameTripCount(IterLoop1, IterLoop2, SE))
					{
						outs()<<"I Loop:\n"<<*IterLoop1<<*IterLoop2<<" hanno lo stesso trip count\n----------------------\n";

						if (areLoopsControlFlowEquivalent(IterLoop1, IterLoop2, DT, PDT))
						{
							outs()<<"Ho trovato due loop Control-Flow Equivalent:\n";
							outs()<<"Il Loop: "<<*IterLoop1<<" domina ---> "<<*IterLoop2<<"\n";
							outs()<<"Il Loop: "<<*IterLoop2<<" post-domina ---> "<<*IterLoop1<<"----------------------\n";

							loopFusion(IterLoop1, IterLoop2);
						}		
					}
				}			
			}
		}
	}

	loops.clear();
  	return PreservedAnalyses::none();
}

