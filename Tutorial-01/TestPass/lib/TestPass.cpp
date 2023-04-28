#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

  class TestPass final : public PassInfoMixin<TestPass> 
  {
  public:
    PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
    {
      for(auto itMod = M.begin(); itMod != M.end(); ++itMod)
      {
        Function& func = *itMod;
        FunctionType& funcType = *func.getFunctionType();

        // Function name & params
        // ---------------------- 
        const char* funcName = func.getName().data(); 
        const int funcNumParams = funcType.getNumParams();

        outs() << "FuncName=" << funcName << ", NumParams=" << funcNumParams << "\n";
        // if(funcType.isVarArg())
        //   outs() << "+*";
        // outs() << "\n";

        // Basic blocks
        // ---------------------- 
        uint32_t nBlocks = 0;
        uint32_t nInstructions = 0;
        uint32_t nFuncCalls = 0;
        for(auto itBlock = func.begin(); itBlock != func.end(); ++itBlock, ++nBlocks)
        {
          BasicBlock& block = *itBlock;
          
          // Block instruction
          // ---------------------- 
          for(auto itInstr = block.begin(); itInstr != block.end(); ++itInstr, ++nInstructions)
          {
            Instruction& instr = *itInstr;

            outs() << "instruction=[" << instr << "  ]";

            CallInst* callInstr = dyn_cast<CallInst>(&instr);
            if(callInstr)
            {
              Function& called = *callInstr->getCalledFunction();
              outs() << " > Chiamata a funzione: " << called.getName();
              ++nFuncCalls;
            }
            outs() << "\n";
          }

        } 

        outs() << "Numero di chiamate a funzione nello stesso modulo=" << nFuncCalls << "\n";
        outs() << "Numero di Basic Blocks=" << nBlocks << "\n";
        outs() << "Numero di Istruzioni=" << nInstructions << "\n";
        outs() << "--------------------------------------------\n";
      }

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
