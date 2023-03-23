#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockAIP(BasicBlock &B) {
  for(auto &Iter : B){
    outs() << "Istruzione: " << Iter << "\n";
    outs() <<"\tOperandi: ";
    for (auto *I = Iter.op_begin(); I != Iter.op_end(); ++I){
      if (Argument *Arg = dyn_cast<Argument>(I)) {
        outs() << "\t" << *Arg << ": Argument";
      }
      if (ConstantInt *C = dyn_cast<ConstantInt>(I)) {
        outs() << "\t" << *C << ": Constant";
      }
    }
    outs()<<"\n";
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

class AlgebraicIdentityPass final : public PassInfoMixin<AlgebraicIdentityPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {

    for (auto &Iter : M) {
      if (runOnFunctionAIP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

    return PreservedAnalyses::all();
  }
};

