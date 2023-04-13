#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {
class TestPass final : public PassInfoMixin<TestPass> {

private:
  /* Given a function, counts the call instructions for which the callee matches a predicate. */
  int countCallInstrIfCallee(const Function& fun, std::function<bool(const Function*)> predicate) {
    int ans = 0;
    for (const auto &bb : fun.getBasicBlockList())
      for (const auto &inst : bb.getInstList())
        if (const CallInst* call_inst = dyn_cast<CallInst>(&inst))
          if (const Function* callee = call_inst->getCalledFunction())
            if (predicate(callee))
              ++ans;

    return ans;
  }

  /* Given a module, counts the call instructions to a specified function. */
  int countCallsToFunctionInModule(const Module &m, const Function &f) {
    int ans = 0;
    for (const auto &fun : m.getFunctionList())
      ans += countCallInstrIfCallee(fun, [&f](const Function* callee) {
        return callee->getName().equals(f.getName());
      });
    return ans;
  }

  /* Given a function f, counts the call instructions to a function in the same module as f. */
  int countCallsInFunctionToModule(const Function& f, const Module& m) {
    return countCallInstrIfCallee(f, [&m](const Function* callee) {
      if (const Module* called_module = callee->getParent())
        return called_module->getName().equals(m.getModuleIdentifier());
      else
        return false;
    });
  }

public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "Passo di test per il corso di Linguaggi e Compilatori" << "\n\n";

    // TODO: Completare il metodo come indicato per il LAB1.
    
    for (const auto &fun : M.getFunctionList()) {

      // 1. Function name
      outs() << "FUNCTION " << fun.getName() << "\n";
      
      // 2. Argument count
      outs() << "Argument count: " << fun.arg_size();
      if (fun.isVarArg())
        outs() << "+*";
      outs() << "\n";

      // I wasn't sure which one the assignment actually meant, so i just wrote both
      // 3.1. Calls to this function in the current module
      outs() << "Calls to this function in this module: " << countCallsToFunctionInModule(M, fun) << "\n";

      // 3.2. Calls to functions in the current module
      outs() << "Calls in this function to functions of this module: " << countCallsInFunctionToModule(fun, M) << "\n";

      // 4. Basic Block count
      outs() << "Basic block count: " << fun.getBasicBlockList().size() << "\n";

      // 5. Instruction count
      outs() << "Instruction count: " << fun.getInstructionCount() << "\n";

      outs() << "\n";
    }

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
