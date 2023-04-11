#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace
{

  class TestPass final : public PassInfoMixin<TestPass>
  {
  public:
    PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &)
    {
      outs() << "Passo di test per il corso di Linguaggi e Compilatori"
             << "\n\n";

      for (auto iter = M.begin(); iter != M.end(); iter++)
      {
        auto &function = *iter;

        // Nome funzione
        outs() << "Nome funzione: " << function.getName() << "\n";
        auto functionType = function.getFunctionType();

        // Numero parametri funzione
        outs() << "Numero parametri: " << functionType->getNumParams();
        if (functionType->isVarArg())
          outs() << "+*";
        outs() << "\n";

        // Numero di chiamate ad altre funzioni
        uint callCount = 0;
        uint bbCount = 0;
        uint instructionCount = 0;

        for (auto &bb : function)
        {
          bbCount++;
          for (auto &inst : bb)
          {
            if (dyn_cast<CallBase>(&inst) != nullptr)
              callCount++;

            instructionCount++;
          }
        }
        outs() << "Numero chiamate a funzioni: " << callCount << "\n";

        // Numero di Basic Blocks
        outs() << "Numero di BB: " << bbCount << "\n";

        // Numero di Instructions
        outs() << "Numero di Instructions: " << instructionCount << "\n";

        outs() << "\n";
      }
      return PreservedAnalyses::all();
    }
  }; // class TestPass

} // anonymous namespace
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
