#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockAIP(BasicBlock &B) {
  for(auto &Inst : B){

    for(auto *I = Inst.op_begin(); I != Inst.op_end(); ++I){
      int a;
      if (ConstantInt *C = dyn_cast<ConstantInt>(I))
        a = C->getSExtValue();

      if(a == 0 && (Inst.getOpcode() == Instruction::Add || Inst.getOpcode() == Instruction::Sub)){
        outs()<< "Inutile in add/sub\n";

      }
      else if(a == 1 && (Inst.getOpcode() == Instruction::Mul || Inst.getOpcode() == Instruction::SDiv)){
        outs()<< "Inutile in mul/div\n";
      }

    }

    /*outs() << "Istruzione: " << Inst << "\n";
    outs() <<"\tOperandi: ";
    for (auto *I = Inst.op_begin(); I != Inst.op_end(); ++I){
      if (Argument *Arg = dyn_cast<Argument>(I)) {
        outs() << "\t" << *Arg << ": Argument";
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(I)) {
        outs() << "\t" << *C << ": Constant";
      }
    }
    outs()<<"\n";*/
  }
};

bool runOnFunctionAIP(Function &F) {
    bool Transformed = false;

    for (auto &Iter : F) {
      if (runOnBasicBlockAIP(Iter)) {
        Transformed = true;
      }
    }

  return Transformed;
};

PreservedAnalyses  AlgebraicIdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager   &) {
    for (auto &Iter : M) {
      if (runOnFunctionAIP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

  return PreservedAnalyses::all();
}
