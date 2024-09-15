#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>

using namespace llvm;


namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "Passo di test per il corso di Linguaggi e Compilatori"
           << "\n";

	// Itera attraverso il Module (cioè sulle Function).
	for(auto iter = M.begin(); iter != M.end(); ++iter) {
	  // Ottiene la Function. 
	  Function& F = *iter;

	  // Stampa il nome della Function.
	  std::cout << "Function name: " << F.getName().data() << std::endl;

	  // Conta gli argomenti della Function.
	  int arg_counter = 0;
	  for(auto iter = F.arg_begin(); iter != F.arg_end(); ++iter) {
		arg_counter++;
	  }
	  if (F.isVarArg())
	  // Se la Function è variadica, stampa "N+*".
	  std::cout << "Num of args: " << arg_counter << "+*" << std::endl;
	  else {
	  // Altrimenti, stampa il numero esatto di argomenti.
	    std::cout << "Num of args: " << arg_counter << std::endl;	    
	  }
	  
      // Contatori per: numero blocchi, numero function call, numero istruzioni.
	  int num_blocks = 0;
	  int call_counter = 0;
	  // Itera attraverso la Function (cioè sui BasicBlock).
	  for(auto iter_bb = F.begin(); iter_bb != F.end(); ++iter_bb) {
		BasicBlock& BB = *iter_bb;
		num_blocks++;

		for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {
			Instruction& I = *iter_inst;
			if (CallInst *call_inst = dyn_cast<CallInst>(iter_inst))
				call_counter++;
		}
	  }
	  std::cout << "Num of blocks: " << num_blocks << std::endl;
	  // Il numero di istruzioni si può dedurre anche iterando su ciascun
	  // BasicBlock, ma la chiamata della funzione di libreria è più semplice.
	  std::cout << "Num of instr: " << F.getInstructionCount() << std::endl;
	  std::cout << "Num of calls: " << call_counter << std::endl;

	}
	
	// Fine.
	std::cout << "Done" << std::endl;
    return PreservedAnalyses::all();
  }
}; // class TestPass

} // anonymous namespace


extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "TestPass",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "test-pass") {
                    MPM.addPass(TestPass());
                    return true;
                  }
                  return false;
                });
          } 
  };
}
