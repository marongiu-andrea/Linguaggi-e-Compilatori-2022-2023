#include "LocalOpts.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <map>

using namespace llvm;


extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    .APIVersion = LLVM_PLUGIN_API_VERSION,
    .PluginName = "LocalOpts",
    .PluginVersion = LLVM_VERSION_STRING,
    .RegisterPassBuilderCallbacks =
      [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback(
          // Function passes
          [](StringRef Name, FunctionPassManager &FPM,
            ArrayRef<PassBuilder::PipelineElement>) -> bool {
            
            if(Name == "my-loop-fusion")
              FPM.addPass(MyLoopFusionPass());

            else
              return false;
            
            return true;
        });    
      }
  };
}