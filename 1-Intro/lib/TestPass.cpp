#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace llvm;

namespace
{

  class TestPass final : public PassInfoMixin<TestPass>
  {
  public:
    PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &)
    {
      outs() << "\nPasso di test per il corso di Linguaggi e Compilatori"
             << "\n\n";
      outs() << "NOME DEL MODULO: " << M.getName() << "\n\n";

      for (auto iter = M.begin(); iter != M.end(); ++iter)
      {
        //get the current function
        Function &F = *iter;
        auto nBasicBlocks = 0;
        std::string variable_arg = "";

        outs() << "Nome funzione: " << F.getName() << "\n";

        //check if the function accepts a variable  number of arguments
        if (F.isVarArg())
        {
          variable_arg = "+*";
        }
        //get the function's number of passed arguments
        outs() << "Numero parametri: " << F.arg_size() << variable_arg << "\n";
        //count all the basic blocks of the function
        for (auto &basic_block : F)
        {
          nBasicBlocks++;
        }
        outs() << "Numero di chiamate della funzione nel modulo : " << F.getNumUses() << "\n";
        outs()
            << "Numero basic block : " << nBasicBlocks << "\n";
        outs() << "Numero istruzioni : " << F.getInstructionCount() << "\n";
        outs() << "--------------------"
               << "\n";
      }
  
      return PreservedAnalyses::all();
    }
  };
} // class TestPass

// anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo()
{
  return {
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
      }};
}
