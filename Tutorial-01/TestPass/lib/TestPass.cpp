#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

using namespace llvm;

namespace {
class TestPass final : public PassInfoMixin<TestPass> {
  public:
    PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {

      std::map<Function*, std::map<std::string, int>> moduleMap;
      for (Function &function : M.getFunctionList()) {
        moduleMap[&function];
        for (BasicBlock &BB : function) {
          moduleMap[&function]["basicBlocks"]++;
          for(Instruction &inst: BB) {
            moduleMap[&function]["instructions"]++;
            if (isa<CallInst>(inst)) {
              moduleMap[&function]["call"]++;
            }
          }
        }
      }
      for(auto &function : moduleMap) {
        outs() <<"Function name: " << function.first->getName()<<"\n";
        outs() <<"    Number of arguments:      " << function.first->arg_size();
        if(function.first->isVarArg())
          outs() <<"+*";
        outs() <<"\n";
        outs() <<"    Number of calling instr:  " << function.second["call"]<<"\n";
        outs() <<"    Number of basic blocks:   " << function.second["basicBlocks"]<<"\n";
        outs() <<"    Number of instructions:   " << function.second["instructions"]<<"\n";
      }
      return PreservedAnalyses::all();
    }
  };
} 

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
          }
        );
      } 
  };
}
