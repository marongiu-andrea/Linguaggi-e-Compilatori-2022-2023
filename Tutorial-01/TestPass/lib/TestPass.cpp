#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Function.h"

using namespace llvm;

namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "\nPasso di test per il corso di Linguaggi e Compilatori"
           << "\n\n";
            for (auto iter = M.getFunctionList().begin(); iter!=M.getFunctionList().end(); ++iter) 
            {
              Function &F = *iter;
              outs()<< "Nome funzione: ";
              outs()<< F.getName().data();
              outs()<< "\n";

              outs()<< "numero di argomenti: ";
              outs()<< F.getFunctionType()->getNumParams();
              if(F.getFunctionType()->isVarArg())
              {
                outs()<<"+*";
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

  };
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
