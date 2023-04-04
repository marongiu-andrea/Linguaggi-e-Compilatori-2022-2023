#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#include "LocalOpts.h"

using namespace llvm;

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    .APIVersion = LLVM_PLUGIN_API_VERSION,
    .PluginName = "LocalOpts",
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
      [](StringRef Name, ModulePassManager &MPM,ArrayRef<PassBuilder::PipelineElement>) -> bool 
      {
        if (Name == "algebric_identity") 
        {
          MPM.addPass(AlgebricIdentityPass());
          return true;
        }
        if (Name == "strength_reduction") 
        {
          MPM.addPass(TransformPass());
          return true;
        }
        if (Name == "multi_instruction") 
        {
          MPM.addPass(MultiInstructionPass());
          return true;
        }

        return false;
      });
    } // RegisterPassBuilderCallbacks
  }; // struct PassPluginLibraryInfo
}
