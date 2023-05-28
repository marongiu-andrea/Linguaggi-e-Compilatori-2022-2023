#include "LoopFusionPass.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    .APIVersion = LLVM_PLUGIN_API_VERSION,
    .PluginName = "LocalOpts",
    .PluginVersion = LLVM_VERSION_STRING,
    .RegisterPassBuilderCallbacks = [](PassBuilder &PB) { 
      PB.registerPipelineParsingCallback(

      [](StringRef Name, ModulePassManager &MPM,ArrayRef<PassBuilder::PipelineElement>) -> bool 
      {
        if (Name == "loop-fusion") 
        {
          MPM.addPass(LoopFusionPass());
          return true;
        }
        return false;
      });
    } 
  }; 
}


PreservedAnalyses LoopFusionPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) 
{
  return PreservedAnalyses::none();
}
