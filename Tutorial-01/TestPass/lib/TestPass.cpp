#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "\n";

    // TODO: Completare il metodo come indicato per il LAB1.

    for (auto iter = M.getFunctionList().begin(); iter!=M.getFunctionList().end(); ++iter) // For per ciclare le funzioni dei moduli e i moduli sono letti dalla variabile M passata alla funzione
    { // i file che vengono controllati sono fibonacci.c o loop.c in base alla chiamata
      Function &F = *iter;
      outs()<< "Nome funzione: ";
      outs()<< F.getName().data(); // printa il nome della funzione
      outs()<< "\n";

      outs()<< "numero di argomenti: ";
      outs()<< F.getFunctionType()->getNumParams(); // per ottenere il numero degli argomenti
      if(F.getFunctionType()->isVarArg())
      {
        outs()<<"+*"; // output per le funzioni variadiche
      }
      outs()<< "\n";

      int c = 0;
      int cIstruzioni = 0;
      int cbb = 0;

      for (auto bb = F.begin(); bb!=F.end(); ++bb) 
      {
        cbb++;
        for (auto instruction = (*bb).begin(); instruction!=(*bb).end(); ++instruction) 
        {
          cIstruzioni++;
          if (CallInst *callInst = dyn_cast<CallInst>(instruction)) 
          {
            if (Function *calledFunction = callInst->getCalledFunction()) 
            {
              c++;
            }
          }
        }
      }

      outs()<< "numero di chiamate alla funzione: ";
      outs()<< c;
      outs()<< "\n";

      outs()<< "numero di istruzioni: ";
      outs()<< cIstruzioni;
      outs()<< "\n";

      outs()<< "numero di basic block: ";
      outs()<< cbb;
      outs()<<"\n\n";
    }

    outs()<<"\n";

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
