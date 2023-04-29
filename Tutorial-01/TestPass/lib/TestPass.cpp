#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace llvm;

namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "Passo di test per il corso di Linguaggi e Compilatori"
           << "\n";

      // TODO: Completare il metodo come indicato per il LAB1.
      for (auto iter = M.begin(); iter != M.end(); ++iter){
        Function &F = *iter;
        auto nBasicBlocks = 0;
        std::string variable_arg = "";
        
        outs() << "Nome funzione: "<< F.getName() <<"\n";
  
        if(F.isVarArg()){
          variable_arg = "+*";
        }
        outs() << "Numero parametri: "<< F.arg_size() << variable_arg <<"\n";
        for (auto &basic_block : F) {
          nBasicBlocks++;
        }
        
        const CallGraphNode * llvm::CallGraph::operator[](	const Function * 	F	)
        outs() << "Numero basic block : " << nBasicBlocks << "\n";
        outs() << "Numero istruzioni : " <<F.getInstructionCount() << "\n";
        outs() << "--------------------" << "\n";
       }
    
    return PreservedAnalyses::all();
    }                                                                                           
  };
} // class TestPass

 // anonymous namespace

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
