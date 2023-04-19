#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class TestPass final : public PassInfoMixin<TestPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    outs() << "Passo di test per il corso di Linguaggi e Compilatori"
           << "\n";

          for(Function &F : M){
            int bbs=0,inst=0,calls=0,params=0;
            for(BasicBlock &B : F){
              bbs++;
              for(Instruction &I : B){
                inst++;
                if(CallInst *Instance = dyn_cast<CallInst>(&I)){
                  calls++;
                }
              }
            }
            for(auto &arg : F.args()){
              params++;
            }

            outs() << "Funzione: " << F.getName() << "\n";
            outs() << "\tArgomenti: " << params << "\n";
            outs() << "\tChiamate a Funzione: " << calls << "\n";
            outs() << "\tBasicBlocks: " << bbs << "\n";
            outs() << "\tIstruzioni: " << inst << "\n";

          }

          return PreservedAnalyses::all();

    }
  };
}; // class TestPass

// anonymous namespace

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
                });
          } 
  };
}
