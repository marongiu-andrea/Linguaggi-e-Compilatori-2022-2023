#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

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

	}

	virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override 
	{
		outs() << "\nLOOPPASS INIZIATO...\n";

		//Verifica che il loop sia in forma normalizzata
		if ((*L).isLoopSimplifyForm())
		{
			outs()<<"L è un Loop in forma normalizzata\n";
			
			// Controlla che il loop abbia un preheader
			if((*L).getLoopPreheader() != NULL)
			{
				//Dobbiamo fare una variabile basic block dove salvare il ritorno di getlooppreheader
				outs()<<"L ha un preheader\n";
			}
		}



		return false; 
	}
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

