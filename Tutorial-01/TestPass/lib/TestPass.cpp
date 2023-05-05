#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>



using namespace llvm;
namespace {
  // TODO: Completare il metodo come indicato per il LAB1 esercizio 2
  // sta roba deve cercare usando la documentazione
  // 1. Nome funzione
  // 2. numero di argomenti
  // 3. numero di chiamate a funzione nello stesso modulo
  // 4. numero di basic block
  // 5. numero di istruzioni
  // https://www.llvm.org/docs/WritingAnLLVMPass.html
  // https://llvm.org/docs/NewPassManager.html
  // https://llvm.org/doxygen/classllvm_1_1Instruction.html
  // https://llvm.org/doxygen/classllvm_1_1Module.html
  // https://llvm.org/doxygen/dir_e6e1f1f37d351595fa984b942927b205.html
  // https://llvm.org/doxygen/CFGuard_8cpp_source.html
  // https://stackoverflow.com/questions/35370195/llvm-difference-between-uses-and-user-in-instruction-or-value-classes


  /**
   * Interfacce dei passi LLVM pag9/26 002-03 slide backend
   * 
   * LLVM fornisce diverse interfacce per i passi
   * BasicBlockPass: itera sui basic blocks
   * CallGraphSCCPass: itera sui nodi del call graph
   * FunctionPass: itera sulla lista delle funzioni nel modulo
   * LoopPass: itera sui loops, in ordine inverso di nesting
   * ModulePass: generico passo interprocedurale
   * RegionPass: itera sulle SESE regions, in ordine inverso di nesting
   * 
   * Prove dei vari metodi in LLVM
   * User-->Use-->Value
  */
  class TestPass final : public PassInfoMixin<TestPass> {
    private:

    /**
     * @brief Un basic block è un elenco di istruzioni
     * @param B 
     * @return 
     */
    bool runOnBasicBlock(BasicBlock &B) {
      Instruction &Inst1st = *B.begin(), &Inst2nd = *(++B.begin());
      assert(&Inst1st == Inst2nd.getOperand(0));
      outs() << "Prima istruzione: " << Inst1st << "\n";
      outs() << "COME OPERANDO: ";
      Inst1st.printAsOperand(outs(), false);
      outs() << "\n";
      outs() << "I MIEI OPERANDI SONO:\n";
      for (llvm::Use *Iter = Inst1st.op_begin(); Iter != Inst1st.op_end(); ++Iter) {
        Value *Operand = *Iter; 
        if (Argument *Arg = dyn_cast<Argument>(Operand)) {
          outs() << "\t" << *Arg << ": SONO L'ARGOMENTO N. " << Arg->getArgNo() 
            <<" DELLA FUNZIONE " << Arg->getParent()->getName() << "\n";
        }
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) {
          outs() << "\t" << *C << ": SONO UNA COSTANTE INTERA DI VALORE " << C->getValue() << "\n";
        }
      }
      outs() << "LA LISTA DEI MIEI USERS:\n";
      for (Value::user_iterator Iter = Inst1st.user_begin(); Iter != Inst1st.user_end(); ++Iter) 
        outs() << "\t" << *(dyn_cast<Instruction>(*Iter)) << "\n";  
      outs() << "E DEI MIEI USI (CHE E' LA STESSA):\n";
      for (Value::use_iterator Iter = Inst1st.use_begin(); Iter != Inst1st.use_end(); ++Iter) 
        outs() << "\t" << *(dyn_cast<Instruction>(Iter->getUser())) << "\n";
      Instruction *NewInst = BinaryOperator::Create( Instruction::Add, Inst1st.getOperand(0), Inst1st.getOperand(0) );
      //NewInst->insertAfter(&Inst1st);
      //Inst1st.replaceAllUsesWith(NewInst);
      return true;
    }


      bool runOnFunction(Function &F) {
        bool Transformed = false;
        for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
          if (runOnBasicBlock(*Iter))  Transformed = true;
        }
        return Transformed;
      }

    public:
      PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &A) {

        outs() << "Nome File " << M.getSourceFileName(); 
        for (Module::iterator Iter = M.begin(); Iter != M.end(); ++Iter) {
          if (runOnFunction(*Iter)) {
            return PreservedAnalyses::none();
          }
        }
          outs() << '\n';
          return PreservedAnalyses::all();
      } 
  };
} 


/**
 * @brief Classe extern C
 * @return vero, falso 
 */
extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    .APIVersion = LLVM_PLUGIN_API_VERSION,
    .PluginName = "TestPass",
    .PluginVersion = LLVM_VERSION_STRING,
    .RegisterPassBuilderCallbacks = [](PassBuilder &PB) { 
      PB.registerPipelineParsingCallback ( [] 
        (StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) -> bool {
          if (Name == "test-pass") { 
            MPM.addPass(TestPass()); return true; } 
          return false;
        }
      );
    } 
  };
}
