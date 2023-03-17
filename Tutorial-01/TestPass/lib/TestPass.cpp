#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "Passo di test per il corso di Linguaggi e Compilatori"
           << "\n";

    int call_func = 0;

    for(auto iter=M.begin(); iter!=M.end(); ++iter){
      Function &F = *iter;
      //outs() << F.getName() << "\n" << "Numero di argomenti: " << F.arg_size() << "\n";
      
      if (F.isVarArg()){
      	outs() << F.getName() << "\n" << "Numero di argomenti: " << F.arg_size() << "+*\n";
      }
      else {
      	outs() << F.getName() << "\n" << "Numero di argomenti: " << F.arg_size() << "\n";
      }


      int n_basicblock = 0;
      int n_istruzioni = 0;

      for(BasicBlock &BB : F){
        n_basicblock += 1;
        n_istruzioni += BB.size();

        for(Instruction &inst : BB){
          if(isa<CallInst>(&inst))
            call_func += 1;

        }

      }  

      //  OPPURE
      
      /*
      if(F.getEntryCount(true).hasValue()) {
      	outs() << "Numero di Basic Block: " << F.size() << "\nNumero di istruzioni totali: " << F.getInstructionCount() << "\nNumero di chiamate della funzione: " << (F.getEntryCount(true).getValue()).getCount() <<  "\n\n";
      }
      else {
      	outs() << "Numero di Basic Block: " << F.size() << "\nNumero di istruzioni totali: " << F.getInstructionCount() << "\n\n";
      }
      */
      
      outs() << "Numero di chiamate a funzione: " << call_func << "\n";
      outs() << "numero di Basic Block: " << n_basicblock <<"\n" << "Numero di istruzioni: "<< n_istruzioni <<"\n\n";


    }
      // TODO: Completare il metodo come indicato per il LAB1.
    
    //}

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
