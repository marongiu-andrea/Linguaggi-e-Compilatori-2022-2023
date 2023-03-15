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

      // TODO: Completare il metodo come indicato per il LAB1.
      for(auto iterM = M.begin(); iterM != M.end(); ++iterM){
        Function &F = *iterM;
        outs()<<F.getName()<<"\n";
        int contBB=0,contCall=0;
        //Conteggio basic blocks
        for(auto iterF = F.begin(); iterF != F.end(); ++iterF){
          BasicBlock &BB = *iterF;
          contBB++;
          for(auto iterBB = BB.begin(); iterBB != BB.end(); ++iterBB){
            Instruction &Inst = *iterBB;
            if(CallInst *call_inst=dyn_cast<CallInst>(&Inst))
              contCall++;
          }
        }
        outs()<<"\tNumero Basic Blocks: "<<contBB;
        if(F.isVarArg()){
          outs()<<"\n\tNumero Argomenti: "<<F.arg_size()<<"+*";
        }else{
          outs()<<"\n\tNumero Argomenti: "<<F.arg_size();
        }
        outs()<<"\n\tNumero Istruzioni: "<<F.getInstructionCount();
        outs()<<"\n\tFunzioni Chiamate: "<<contCall<<"\n\n";
      }

    return PreservedAnalyses::all();
  }
}; // class TestPass

} // anonymous namespace

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
