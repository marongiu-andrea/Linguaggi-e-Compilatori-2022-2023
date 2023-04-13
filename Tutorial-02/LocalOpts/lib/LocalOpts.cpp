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
          [](StringRef Name, ModulePassManager &MPM,
            ArrayRef<PassBuilder::PipelineElement>) -> bool {

            if (Name == "transform")
              MPM.addPass(TransformPass());

            else if(Name == "multi-instruction-operations")
              MPM.addPass(MultiInstructionOperationsPass());

            else
              return false;

            return true;
        });
        
        PB.registerPipelineParsingCallback(
          // Function passes
          [](StringRef Name, FunctionPassManager &FPM,
            ArrayRef<PassBuilder::PipelineElement>) -> bool {
            
            if(Name == "algebraic-identity")
              FPM.addPass(AlgebraicIdentityPass());

            else if(Name == "strength-reduction")
              FPM.addPass(StrengthReductionPass());

            else
              return false;
            
            return true;
        });    
      }
  };
}