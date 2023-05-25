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
      unsigned int nFunc,nBlock,nIstr;
      nFunc=nBlock=nIstr=0;

      for (auto iter=M.begin(); iter != M.end(); ++iter){
        printf("---------Funzione %u---------\n", nFunc);
        Function &F = *iter;
        printf("Nome: %s* \n",
                F.getName().str());
        printf("N parametri: %lu \n",
                F.arg_size());
        nFunc++;
        ///////////////
        for (BasicBlock &B : F){
          nBlock++;
          for (Instruction &I : B){
              /*if(CallInst *call_inst = dynamic_cast<CallInst>(I))
                nIstr++;*/
              if (I.getOpcode() == Instruction::Call){
                nIstr++;
                /*CallInst& callInstr = static_cast<CallInst&>(I);
                if (callInstr.getCalledFunction()->getParent()->getModuleIdentifier()==M.getModuleIdentifier()){
                  nIstr++;
                }*/
              }
          };//fine loop instruction
        };//fine loop block
      };
      printf("--------Modulo--------\n");
      printf("Numero blocchi: %u\n", nBlock);
      printf("Numero istruzioni: %u\n", M.getInstructionCount());
      printf("Numero chiamate: %u\n", nIstr);
      //printf("Numero istruzioni: %u\n", nIstr); //non serve contarle uno ad uno
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
