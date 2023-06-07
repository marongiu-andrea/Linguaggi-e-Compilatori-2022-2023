#include "LocalOpts.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;


extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    .APIVersion = LLVM_PLUGIN_API_VERSION, .PluginName = "LocalOpts",
    .PluginVersion = LLVM_VERSION_STRING,
    .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
          // CREO LO STUB per il mio TestPass
          //
          // RICORDA: Posso usare (coi relativi Pass Managers)
          // -------------------------------------------------
          // ModulePass
          // CallGraphSCCPass
          // FunctionPass
          // LoopPass
          // RegionPass
          // BasicBlockPass

          [](StringRef Name, FunctionPassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) -> bool {
            if(Name == "loopFusion"){
              MPM.addPass(LoopFusionPass());
              return true;
            }
            return false;
              });
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}
