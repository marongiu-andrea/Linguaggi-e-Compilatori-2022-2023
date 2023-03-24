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
                  if(Name == "algebraic-identity"){
                    MPM.addPass(AlgebraicIdentityPass());
                    return true;
                  }
                  return false;
                }
          );
          PB.registerPipelineParsingCallback(
               [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if(Name == "strength-reduction"){
                    MPM.addPass(StrengthReductionPass());
                    return true;
                  }
                  return false;
                }
          );
          PB.registerPipelineParsingCallback(
               [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if(Name == "multi-instruction-operations"){
                    MPM.addPass(MultiInstructionOperationsPass());
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