#include "LocalOpts.h"
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "LocalOpts",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
		           [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "transform") {
                    MPM.addPass(TransformPass());
                    return true;
                  }
                  return false;
                });
          PB.registerPipelineParsingCallback(
               [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if(Name == "Algebraic_identity"){
                    MPM.addPass(algerbraic_pass());
                    return true;
                  }
                  return false;
                }
          );
          PB.registerPipelineParsingCallback(
               [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if(Name == "Strength_reduction"){
                    MPM.addPass(strength_pass());
                    return true;
                  }
                  return false;
                }
          );
          PB.registerPipelineParsingCallback(
               [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if(Name == "Multi_instruction"){
                    MPM.addPass(multi_pass());
                    return true;
                  }
                  return false;
                }
          );


                
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}
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
    // TODO: Implementare gli stub per
		  // Algebraic Identity
		  // Strength Reduction
		  // Multi-instruction Operations