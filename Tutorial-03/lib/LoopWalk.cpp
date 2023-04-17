#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

// opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk test/Loop.ll -disable-output

namespace {

class LoopWalkPass final : public LoopPass 
{
	public:
	static char ID;

	LoopWalkPass() : LoopPass(ID) {}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const override 
	{
		// Imposta il Dominator Tree e le Loop Info come necessarie all'esecuzione del passo corrente
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
	}

	virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override 
	{
		// Ottiene riferimenti al Dominator Tree e alle Loop Info
		DominatorTree * DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
		LoopInfo * LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

		outs()<<"\nLOOPPASS INIZIATO...\n";

		// ----------------- ESERCIZIO 1 ------------------
		// Verifica che il loop sia in forma normalizzata
		if ((*L).isLoopSimplifyForm())
		{
			outs()<<"Il loop è in forma normalizzata\n\n";
			
			// Controlla che il loop abbia un preheader
			BasicBlock * headerBlock = (*L).getLoopPreheader();
			if(headerBlock != NULL)
			{
				outs()<<"Il loop ha un preheader: ";

				// Stampa del preheader
				outs()<<*headerBlock<<"\n";
				
				// Stampa dei Basic Block del loop
				outs()<<"Basic Block del loop: ";
				for (auto &BBIter : (*L).getBlocks())
					outs()<<*BBIter<<"\n";
				
				// ----------------- ESERCIZIO 2 ------------------
				// Itera su tutte le istruzioni del loop
				for (auto &BBIter : (*L).getBlocks())
				{
					for (auto &InstIter : *BBIter)
					{
						// Identifica l'istruzione SUB
						if (InstIter.getOpcode() == Instruction::Sub)
						{
							ConstantInt * firstOperand = dyn_cast<ConstantInt>(InstIter.getOperand(0));
							ConstantInt * secondOperand = dyn_cast<ConstantInt>(InstIter.getOperand(1));

							// Controlla gli operandi della SUB e per quelli non costanti stampa l'istruzione ad essi associati
							if (!firstOperand)
							{
								Instruction * firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0));
								outs()<<"Istruzione del primo operando:"<<*firstOperandInstruction<<"\n";

								// Stampa il Basic Block che contiene l'istruzione 
								outs()<<"Basic Block che contiene l'istruzione:"<<*((*firstOperandInstruction).getParent());
							}
							else
								outs()<<"Il primo operando è una costante di valore "<<(*firstOperand).getValue()<<"\n";
							
							if (!secondOperand)
							{
								Instruction * secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(1));
								outs()<<"Istruzione del secondo operando:"<<*secondOperandInstruction<<"\n";

								// Stampa il Basic Block che contiene l'istruzione 
								outs()<<*((*secondOperandInstruction).getParent());
							}
							else
								outs()<<"Il secondo operando è una costante di valore "<<(*secondOperand).getValue()<<"\n";	
						}
					}
				}
			}
			else
				outs()<<"Il loop NON ha un preheader\n";
		}
		else
			outs()<<"Il loop NON è in forma normalizzata\n\n";

		return false; 
	}
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

