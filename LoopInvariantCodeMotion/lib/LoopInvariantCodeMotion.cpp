#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Analysis/LoopIterator.h"

using namespace llvm;

#include <iostream>
#include <map>
#include <vector>

// opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -disable-output
// opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -o test/LoopInvariantCodeMotion.optimized.bc
// llvm-dis test/LoopInvariantCodeMotion.optimized.bc -o test/LoopInvariantCodeMotion.optimized.ll

namespace {

class LoopInvariantCodeMotionPass final : public LoopPass 
{
	public:
	static char ID;
	std::map<Instruction*, bool> loopInvariantMap; // Definisce quali istruzioni sono loop-invariant
	std::vector<Instruction*> candidateMovableInstructions; // Contiene le istruzioni candidate alla code motion
	std::vector<Instruction*> movableInstructions; // Contiene le istruzioni sulle quali si può eseguire la code motion
	LoopInvariantCodeMotionPass() : LoopPass(ID) {}

	virtual bool isLoopSuitableToLICM(Loop *L)
	{
		// Controlla che il loop sia in forma normalizzata, che abbia un preheader e uno o più exit blocks
		if ((*L).isLoopSimplifyForm() && (*L).getLoopPreheader() && (!(*L).hasNoExitBlocks()))
			return true;
		else 
			return false;
	}

	virtual void printLoopInvariantInstructions()
	{
		outs()<<"--------------------------------\n"<<"Istruzioni loop-invariant:\n";
		for (auto &itr: loopInvariantMap)
		{
			if (itr.second)
				outs()<<*(itr.first)<<"\n";
		}
	}

	virtual bool findLoopInvariantInstructions(Loop *L)
	{
		outs()<<"--------------------------------\n"<<"Ricerca delle istruzioni Loop-invariant:\n";
		for (auto &BBIter : (*L).getBlocks())
		{
			for (auto &InstIter : *BBIter)
			{
				loopInvariantMap[&InstIter] = false;

				if (InstIter.isBinaryOp())
				{
					ConstantInt * firstOperand = dyn_cast<ConstantInt>(InstIter.getOperand(0));
					ConstantInt * secondOperand = dyn_cast<ConstantInt>(InstIter.getOperand(1));

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
							// Se il primo operando si riferisce a un'istruzione che non è definita all'interno del loop
							if (((*L).contains(firstOperandInstruction)) == false)
							{
								outs()<<(*L).contains(firstOperandInstruction)<<"\n";
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché la definizione del primo operando non è contenuta nel loop\n";
								loopInvariantMap[&InstIter] = true;
							}

							// Se il primo operando si riferisce a un'istruzione che è già loop-invariant
							if (loopInvariantMap[firstOperandInstruction] == true)
							{
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché il primo operando è a sua volta loop invariant\n";
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

							// Se il secondo operando si riferisce a un'istruzione che è già loop-invariant
							if (loopInvariantMap[secondOperandInstruction] == true)
							{
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché il secondo operando è a sua volta loop invariant\n";
								loopInvariantMap[&InstIter] = true;
							}
						}
						else // Il secondo operando non ha un'istruzione ad esso associato, pertanto viene definito fuori dal Loop
						{
							outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché il secondo operando è stato definito fuori dal loop\n";
							loopInvariantMap[&InstIter] = true;
						}
					}

					// Se nessuno degli operandi è una costante
					if (!firstOperand && !secondOperand)
					{
						Instruction *firstOperandInstruction;
						Instruction *secondOperandInstruction;
						// Se entrabmi i cast a Instruction degli operandi 0 e 1 hanno successo -> ci sono definizioni delle istruzioni corrispondenti gli operandi 0 e 1
						if((firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0))) 
							&& (secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(1))))
						{
							if (((*L).contains(firstOperandInstruction)) == false && ((*L).contains(secondOperandInstruction)) == false)
							{
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché entrambe le definizioni degli operandi non sono contenute nel loop\n";
								loopInvariantMap[&InstIter] = true;
							}

							if (((*L).contains(firstOperandInstruction)) == true && ((*L).contains(secondOperandInstruction)) == false)
							{
								if(loopInvariantMap[firstOperandInstruction] == true)
								{
									outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché la definizione del primo operando è già loop invariant e il secondo operando è definito fuori dal loop\n";
									loopInvariantMap[&InstIter] = true;
								}
							}
							if (((*L).contains(firstOperandInstruction)) == false && ((*L).contains(secondOperandInstruction)) == true)
							{
								if(loopInvariantMap[secondOperandInstruction] == true)
								{
									outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché la definizione del secondo operando è già loop invariant e il primo operando è definito fuori dal loop\n";
									loopInvariantMap[&InstIter] = true;
								}								
							}
							if (((*L).contains(firstOperandInstruction)) == true && ((*L).contains(secondOperandInstruction)) == true)
							{
								if(loopInvariantMap[firstOperandInstruction] == true && loopInvariantMap[secondOperandInstruction] == true)
								{
									outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché le definizioni di entrambi gli operandi sono loop invariant\n";
									loopInvariantMap[&InstIter] = true;
								}								
							}
						}

						// Se solo il cast a Instruction dell'operando 1 ha successo -> c'è solo una definizione dell'istruzione corrispondente all'operando 1
						if(!(firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0))) && (secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(1))))
						{
							if (loopInvariantMap[secondOperandInstruction] == true)
							{
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché l'istruzione del secondo operando è loop invariant\n";
								loopInvariantMap[&InstIter] = true;
							}
						}
						// Se solo il cast a Instruction dell'operando 0 ha successo -> c'è solo una definizione dell'istruzione corrispondente all'operando 0
						if((firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0))) && !(secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(1))))
						{
							if (loopInvariantMap[firstOperandInstruction] == true)
							{
								outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché l'istruzione del primo operando è loop invariant\n";
								loopInvariantMap[&InstIter] = true;
							}
						}
						// Se il cast a Instruction di entrambi gli operandi non ha successo -> non ci sono definizioni delle istruzioni corrispondendi agli operandi 0 e 1
						if(!(firstOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0))) && !(secondOperandInstruction = dyn_cast<Instruction>(InstIter.getOperand(0))))
						{
							outs()<<"L'istruzione "<<InstIter<<" è loop invariant perché entrambe le definizioni degli operandi non sono contenute nel loop\n";
							loopInvariantMap[&InstIter] = true;
						}
					}
				}			
			}
		}
		return true;
	}

	virtual void findCandidateMovableInstructions(Loop *L, DominatorTree * DT)
	{
		
		bool dominatesAllExits;
		bool deadAfterExit;
		bool dominatesAllUses;
		SmallVector<BasicBlock *> exitBlocks; // Vettore contenente i basic block che sono uscite del loop
		(*L).getExitBlocks(exitBlocks);

		outs()<<"--------------------------------\n"<<"Ricerca delle istruzioni candidate alla Code Motion:\n";

		// Itera su tutte le istruzioni loop-invariant
		for(auto iter = loopInvariantMap.begin(); iter != loopInvariantMap.end(); ++iter)
		{
			if (iter->second == true)
			{
				dominatesAllExits = true;
				deadAfterExit = true;
				dominatesAllUses = true;
				for (auto *exitBB : exitBlocks) 
				{
					// Se il blocco contentente un'istruzione loop-invariant NON domina un'uscita -> l'istruzione non è movable
					if (!(*DT).dominates((*DT).getNode(iter->first->getParent()), (*DT).getNode(exitBB)))
					{
						outs()<<*(iter->first)<<" NON domina un'uscita\n";
						dominatesAllExits = false;
					}
				}
					
				// Scorre la lista degli usi delle variabili loop-invariant
				for (auto Iter = (iter->first)->user_begin(); Iter != (iter->first)->user_end(); ++Iter)
				{
					Instruction * inst = dyn_cast<Instruction>(*Iter);	

					// Se la variabile è usata in blocchi che NON fanno parte del loop -> la variabile non è dead all'uscita del loop
					// 	-> Dato che si stanno scorrendo gli usi dell'istruzione iter->first, se un uso non è all'interno del
					//		Loop, significa necessariamente che sono esterni
					if (!(*L).contains(inst))
					{
						outs()<<*(iter->first)<<" NON è dead all'uscita del loop\n";
						deadAfterExit = false;
					}
					
					// Se il blocco dove viene definita un'istruzione loop-invariant non domina un suo uso
					if (!(*DT).dominates((*DT).getNode(iter->first->getParent()), (*DT).getNode(inst->getParent())))
					{
						outs()<<*(iter->first)<<" NON domina un suo uso\n";
						dominatesAllUses = false;
					}
				}
			
				if (deadAfterExit)
					outs()<<*(iter->first)<<" è DEAD all'uscita del loop\n";

				/*
				Un'istruzione candidata movable è:
				- Loop-invariant
				- Si trovano in blocchi che dominano tutte le uscite del loop oppure la variabile definita 
					dall'istruzione è dead fuori dal loop
				- Assegnano un valore a variabili non assegnate altrove nel loop ("Gratis" con SSA)
				- Si trovano in blocchi che dominano tutti i blocchi nel loop che usano la variabile
					a cui si sta assegnando un valore
				*/
				if ((dominatesAllExits || deadAfterExit) && dominatesAllUses)
					candidateMovableInstructions.push_back(iter->first);
			}
		}
	}

	virtual void printInstructions(std::vector<Instruction*> instVector, std::string str)
	{
		outs()<<"--------------------------------\n"<<"Istruzioni "<<str<<":\n";
		for (auto &inst: instVector)
			outs()<<*(inst)<<"\n";
	}

	virtual void codeMotion(Loop *L)
	{
		outs()<<"--------------------------------\n"<<"Code Motion:\n";

		BasicBlock * preHeader = (*L).getLoopPreheader();
				
		Instruction * preHeaderTerminator = preHeader->getTerminator();
		for (auto inst : movableInstructions)
		{
			outs()<<"Sposto l'istruzione "<<*inst<<" nel preheader del loop\n";
			inst->moveBefore(preHeaderTerminator);
		}
	}

	virtual void findMovableInstructions(Loop *L, LoopInfo * LI)
	{
		/*
		Eseguire una ricerca depth-first dei blocchi:
		• Spostare l’istruzione candidata nel preheader se tutte le istruzioni
			invarianti da cui questa dipende sono state spostate
		*/
		LoopBlocksDFS DFS(L);
		DFS.perform(LI);

		bool * operandAllowsToCodeMotion;
		
		// Itera sulle istruzioni candidate alla code motion
		for (auto candidateInst : candidateMovableInstructions)
		{
			operandAllowsToCodeMotion = new bool((*candidateInst).getNumOperands()); // False quando un operando non permette la Code Motion

			for (int i = 0; i < (*candidateInst).getNumOperands(); ++i)
				operandAllowsToCodeMotion[i] = false;

			// Itera sulle istruzioni di ogni Basic Block del Loop in ordine Depth-first
			for (auto BB = DFS.beginRPO(); BB != DFS.endRPO(); ++BB) 
			{
				BasicBlock *B = *BB;
				
				for (auto &instBB : *B)
				{
					// Se nel Basic Block trova un'istruzione candidata alla code motion
					if (candidateInst == &instBB)
					{
						// Itera sugli operandi dell'istruzione candidata
						for (int numOperand = 0; numOperand < instBB.getNumOperands(); ++numOperand)
						{
							ConstantInt * constantOperand = dyn_cast<ConstantInt>(instBB.getOperand(numOperand));
							// Se l'operando di posizione numOperand non è una costante
							if (!constantOperand)	
							{	
								Instruction * operandInstruction;
								// Se l'operando di posizione numOperand non ha un'istruzione associata 
								//	-> l'operando numOperand permette la Code Motion perché viene dall'esterno
								if(!(operandInstruction = dyn_cast<Instruction>(instBB.getOperand(numOperand))))
									operandAllowsToCodeMotion[numOperand] = true;
								else
								{
									for (auto movableInst : movableInstructions)
									{
										operandInstruction = dyn_cast<Instruction>(instBB.getOperand(numOperand));
										if (operandInstruction == movableInst)
											operandAllowsToCodeMotion[numOperand] = true;
									}
								}
							}
							else // Se è una costante -> l'operando numOperand permette la Code Motion
								operandAllowsToCodeMotion[numOperand] = true;
						}
					}
				}
			}

			if (operandAllowsToCodeMotion[0]  && operandAllowsToCodeMotion[1])
				movableInstructions.push_back(candidateInst);
		}
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const override 
	{
		// Imposta il Dominator Tree e le Loop Info come necessarie all'esecuzione del passo corrente
		AU.addRequired<DominatorTreeWrapperPass>();
		AU.addRequired<LoopInfoWrapperPass>();
	}

	virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override 
	{
		outs()<<"Passo Loop Invariant Code Motion\n";

		// Ottiene riferimenti al Dominator Tree e alle Loop Info
		DominatorTree * DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
		LoopInfo * LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

		if (isLoopSuitableToLICM(L))
		{
			findLoopInvariantInstructions(L);	
			printLoopInvariantInstructions();

			findCandidateMovableInstructions(L, DT);
			printInstructions(candidateMovableInstructions, "candidate alla Code Motion");

			findMovableInstructions(L, LI);
			printInstructions(movableInstructions, "movable");

			codeMotion(L);
		}
		return false; 
	}
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-invariant-code-motion",
											"loop invariant code motion");

} // anonymous namespace