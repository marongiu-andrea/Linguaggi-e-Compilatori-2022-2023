#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockAIP(BasicBlock &B) {
  for(auto &Inst : B){

    for(auto *I = Inst.op_begin(); I != Inst.op_end(); ++I){
      int a;
      Argument *Arg;
      if (ConstantInt *C = dyn_cast<ConstantInt>(I))
        a = C->getSExtValue();
      else
        Arg = dyn_cast<Argument>(I);

      if(a == 0 && (Inst.getOpcode() == Instruction::Add || Inst.getOpcode() == Instruction::Sub)){
        outs()<< "Inutile in add/sub: "<< Inst <<"\n";
        Inst.replaceAllUsesWith(Arg);

      }
      else if(a == 1 && (Inst.getOpcode() == Instruction::Mul || Inst.getOpcode() == Instruction::SDiv)){
        outs()<< "Inutile in mul/div\n"<< Inst <<"\n";
        //Inst.replaceAllUsesWith(Arg);
        //Questa cosa fa crashare llvm, DA FIXARE
      }
    }
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
