#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <map>

using namespace llvm;

// opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -disable-output

//dobbiamo scrivere la nostra funzione per capire se uno statement è loop invariant
//calcolare i dominatori -> si possono usare le istruzioni che llvm mette a disposizione
//Trovare le uscite del loop
//Trovare le istruzione candidate alla code motion

//SI trovano in blocchi che dominano tutte le uscite del loop -> rilassabile se la 
//		variabile definita dall'istruzione è dead all'uscita del loop

/*
1)Recuperare info

Primo sweep del loop: -> se 
2)Scrivere un pezzo di codice che ci dice se l'istruzione è loop invariant (NO API LLVM)
3)Scrivere una funzione isSafeToMove()

Secondo sweep in cui si spostano effettivamente le istruzioni:
-> adesso tutte le istruzioni adatte sono marchiate per code motion
4)Eseguite la code motion
*/

namespace {

class LoopInvariantCodeMotionPass final : public LoopPass 
{
	public:
	static char ID;

	LoopInvariantCodeMotionPass() : LoopPass(ID) {}

	virtual bool findLoopInvariantInstructions(Loop *L)
	{
		for (auto &BBIter : (*L).getBlocks())
		{
			for (auto &InstIter : *BBIter)
			{
				if (InstIter.isBinaryOp())
				{
					ConstantInt * firstOperand = dyn_cast<ConstantInt>(InstIter.getOperand(0));
					ConstantInt * secondOperand = dyn_cast<ConstantInt>(InstIter.getOperand(1));

					// Se entrambi gli operandi sono delle costanti
					if (firstOperand && secondOperand)
					{
						outs()<<"L'istruzione "<<InstIter<<" è loop invariant -> entrambi gli operandi sono costanti\n";
					}

					// Se solo il secondo operando è una costante
					if (!firstOperand && secondOperand)
					{
						if(Instruction * firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0)))
						{
							outs()<<"Il primo operando è un'istruzione "<<*firstOperandInstruction<<"\n";
							if ((*L).contains(firstOperandInstruction) == true);
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant\n";

						}
					}

					// Se solo il primo operando è una costante
					if (firstOperand && !secondOperand)
					{
						outs()<<"///\n";
					}
					//outs()<<"Operando 0: "<<*(InstIter.getOperand(0))<<"\n";
					//outs()<<"Operando 1: "<<*(InstIter.getOperand(1))<<"\n";
					//outs()<<"Operando 0 è loop invariant: "<<(*L).isLoopInvariant(InstIter.getOperand(0))<<"\n";
					//outs()<<"Operando 1 è loop invariant: "<<(*L).isLoopInvariant(InstIter.getOperand(1))<<"\n";
					outs()<<"---------------------------------------\n";


				}
				
			}
		}

		return true;
	}

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
		outs()<<"Passo Loop Invariant Code Motion\n";

		
		findLoopInvariantInstructions(L);

		

		return false; 
	}
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-invariant-code-motion",
											"loop invariant code motion");

} // anonymous namespace

