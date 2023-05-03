#include "LocalOpts.h"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using namespace llvm;


/**
 * @brief Classe extern C
 * @return vero, falso 
 * 
 * CREO LO STUB per il mio TestPass
 * 
 * RICORDA: Posso usare (coi relativi Pass Managers)
 * -------------------------------------------------
 * ModulePass
 * CallGraphSCCPass
 * FunctionPass
 * LoopPass
 * RegionPass
 * BasicBlockPass
 * 
 * TODO: Implementare gli stub per
 * Algebraic Identity
 * Strength Reduction
 * Multi-instruction Operations MultiInstruction
 */
extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "LocalOpts",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
        PB.registerPipelineParsingCallback( []
              (StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) -> bool {
              if (Name == "transform") { MPM.addPass(TransformPass()); return true; }
              if (Name == "algebraicidentity") { MPM.addPass(AlgebraicIdentity()); return true; }
              if (Name == "strenghtreduction") { MPM.addPass(Sred()); return true; }
              if (Name == "multiinstruction") { MPM.addPass(MultiInstruction()); return true; }
              return false;                           // nome della classe in MyOpt.cc,.h
            });
          } 
  };
}
