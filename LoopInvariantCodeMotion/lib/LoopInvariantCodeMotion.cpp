#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

#include <iostream>
#include <map>

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
	std::map<Instruction*, bool> loopInvariantMap;

	LoopInvariantCodeMotionPass() : LoopPass(ID) {}

	virtual void printLoopInvariantInstructions()
	{
		outs()<<"Istruzioni loop-invariant:\n";
		for (auto &itr: loopInvariantMap)
		{
			if (itr.second)
				outs()<<*(itr.first)<<"\n";
		}
	}

	virtual bool findLoopInvariantInstructions(Loop *L)
	{
		for (auto &BBIter : (*L).getBlocks())
		{
			for (auto &InstIter : *BBIter)
			{
				loopInvariantMap[&InstIter] = false;

				if (InstIter.isBinaryOp())
				{
					ConstantInt * firstOperand = dyn_cast<ConstantInt>(InstIter.getOperand(0));
					ConstantInt * secondOperand = dyn_cast<ConstantInt>(InstIter.getOperand(1));
					//outs()<<InstIter<<"\n";
					//outs()<<InstIter.getOperand(0)<<"\n";
					//outs()<<InstIter.getOperand(1)<<"\n";
					// Se entrambi gli operandi sono delle costanti
					if (firstOperand && secondOperand)
					{
						outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché entrambi gli operandi sono costanti\n";
						loopInvariantMap[&InstIter] = true;
					}

					// Se solo il secondo operando è una costante
					if (!firstOperand && secondOperand)
					{

						// Se il cast a Instruction dell'operando 0 ha successo -> c'è una definizione dell'istruzione corrsipondente all'operando 0
						if(Instruction * firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0)))
						{
							if (((*L).contains(firstOperandInstruction)) == false)
							{
								outs()<<(*L).contains(firstOperandInstruction)<<"\n";
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché la definizione del primo operando non è contenuta nel loop\n";
								loopInvariantMap[&InstIter] = true;
							}

						}
						else // Il primo operando non ha un'istruzione ad esso associato, pertanto viene definito fuori dal Loop
						{
							outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché il primo operando è stato definito fuori dal loop\n";
							loopInvariantMap[&InstIter] = true;
						}
					}

					// Se solo il primo operando è una costante
					if (firstOperand && !secondOperand)
					{
						// Se il cast a Instruction dell'operando 1 ha successo -> c'è una definizione dell'istruzione corrsipondente all'operando 1
						if(Instruction * secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(1)))
						{
							if (((*L).contains(secondOperandInstruction)) == false)
							{
								outs()<<(*L).contains(secondOperandInstruction)<<"\n";
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché la definizione del secondo operando non è contenuta nel loop\n";
								loopInvariantMap[&InstIter] = true;
							}

						}
						else // Il secondo operando non ha un'istruzione ad esso associato, pertanto viene definito fuori dal Loop
						{
							outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché il secondo operando è stato definito fuori dal loop\n";
							loopInvariantMap[&InstIter] = true;
						}
					}

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
		printLoopInvariantInstructions();


		

		return false; 
	}
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-invariant-code-motion",
											"loop invariant code motion");

} // anonymous namespace

