#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBasicBlockSRP(BasicBlock &B) {
    for(auto &Inst : B){
        if(Inst.getOpcode() == Instruction::Mul){
            ConstantInt *op1 = dyn_cast<ConstantInt>(Inst.getOperand(0));
            ConstantInt *op2 = dyn_cast<ConstantInt>(Inst.getOperand(1));
            if (op1 && !op1->getValue().isOneValue() && isPowerOf2_32(op1->getZExtValue())){
                outs()<<"Riduzione di mul per: "<<Inst<<"\n";
                Instruction *red_inst = BinaryOperator::CreateShl(Inst.getOperand(1), ConstantInt::get(op1->getType(),op1->getValue().logBase2()));
                red_inst->insertAfter(&Inst);
                Inst.replaceAllUsesWith(red_inst);
            }
            else if(op2 && !op2->getValue().isOneValue() && isPowerOf2_32(op2->getZExtValue())){
                outs()<<"Riduzione di mul per: "<<Inst<<"\n";
                Instruction *red_inst = BinaryOperator::CreateShl(Inst.getOperand(0), ConstantInt::get(op2->getType(),op2->getValue().logBase2()));
                red_inst->insertAfter(&Inst);
                Inst.replaceAllUsesWith(red_inst);

            }
        }
    }
}

bool runOnFunctionSRP(Function &F) {
    bool Transformed = false;

    for (auto &Iter : F) {
      if (runOnBasicBlockSRP(Iter)) {
        Transformed = true;
      }
    }
  return Transformed;
};

PreservedAnalyses  StrengthReductionPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager   &) {
    for (auto &Iter : M) {
      if (runOnFunctionSRP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

  return PreservedAnalyses::all();
}
