#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

  /*  
    opt -load-pass-plugin=./libTestPass.so \
    -passes=test-pass test/LoopO0.ll -o test/LoopTestPass.ll
  */

  class TestPass final : public PassInfoMixin<TestPass> {
  public:
    PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
    {
      outs() << "Passo di test per il corso di Linguaggi e Compilatori\n";

      for(auto it = M.begin(); it != M.end(); ++it)
      {
        Function& func = *it;
        FunctionType& funcType = *func.getFunctionType();
        
        // if(func.isDeclaration())
        //   outs() << "func " << func << " IS declaration" << "\n";
        // else
        //   outs() << "func " << func << " IS NOT declaration" << "\n";
        const char* funcName = func.getName().data(); 
        const int funcNumParams = funcType.getNumParams();

        outs() << funcName << " " << funcNumParams << "\n";

      }

      // TODO: Completare il metodo come indicato per il LAB1.
      return PreservedAnalyses::all();
    }
  }; // class TestPass

}; // anonymous namespace

 
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
