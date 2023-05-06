#include "LocalOpts.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() 
{
  	return 
	{
		.APIVersion = LLVM_PLUGIN_API_VERSION,
		.PluginName = "LocalOpts",
		.PluginVersion = LLVM_VERSION_STRING,
		.RegisterPassBuilderCallbacks =
          	[](PassBuilder &PB) 
			{
            	PB.registerPipelineParsingCallback(

				[](StringRef Name, FunctionPassManager &MPM,
						ArrayRef<PassBuilder::PipelineElement>) -> bool {
						if (Name == "loopfusion") {
							MPM.addPass(LoopFusionPass());
							return true;
						}
						return false;
						});
          	}
  	};        
}
