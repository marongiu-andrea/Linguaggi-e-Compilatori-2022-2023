#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

#define SEPARATOR "-----------------------------------------------------"
using namespace llvm;

namespace 
{
	class TestPass final : public PassInfoMixin<TestPass> 
	{
		public:
			PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
			{
				outs()<<"Passo di test per il corso di Linguaggi e Compilatori\n"<<SEPARATOR<<"\n";
				int bbCount = 0;
				int instCount = 0;
				int callCount = 0;
				
				for (auto iterFun = M.begin(); iterFun != M.end(); ++iterFun) // Itera sugli elementi del Module
				{
					Function &F = *iterFun;
					outs()<<"Nome funzione: "<<F.getName()<<"\n";
					if (F.isVarArg())
						outs()<<"Numero argomenti funzione "<<F.getName()<<": "<<F.arg_size()<<"+*\n";
					else
						outs()<<"Numero argomenti funzione "<<F.getName()<<": "<<F.arg_size()<<"\n";
					
					for (auto iterBB = F.begin(); iterBB != F.end(); ++iterBB) // Itera sugli elementi della Function
					{
						BasicBlock &BB = *iterBB;
						++bbCount;

						for (auto iterInst = BB.begin(); iterInst != BB.end(); ++iterInst) // Itera sugli elementi del Basic Block
						{
							Instruction &inst = *iterInst;
							++instCount;
							
							// Downcasting per ricavare il numero di chiamate di funzione
							if (CallInst * call_inst = dyn_cast<CallInst>(&inst))
								++callCount;
						}
					}	
					outs()<<SEPARATOR<<"\n";
				}

				outs()<<"Numero totale di Basic Block: "<<bbCount<<"\n";
				outs()<<"Numero totale di Instruction: "<<instCount<<"\n";
				outs()<<"Numero totale di chiamate di funzione nello stesso modulo: "<<callCount<<"\n";
							
				return PreservedAnalyses::all();
			}

	}; // class TestPass
}; // anonymous namespace


extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() 
{
	return 
  	{
		.APIVersion = LLVM_PLUGIN_API_VERSION,
		.PluginName = "TestPass",
		.PluginVersion = LLVM_VERSION_STRING,
		.RegisterPassBuilderCallbacks =
			[](PassBuilder &PB) 
			{
				PB.registerPipelineParsingCallback(
					[](StringRef Name, ModulePassManager &MPM,
					ArrayRef<PassBuilder::PipelineElement>) -> bool 
					{
						if (Name == "test-pass") 
						{
							MPM.addPass(TestPass());
							return true;
						}
						return false;
					});
			} 
  	};
}
