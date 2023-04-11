#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <vector>

using namespace llvm;

bool runOnBasicBlockMIP(BasicBlock &B) {
    std::vector<Instruction*> toDel;
    for(auto &Inst : B){
        if (Inst.getNumOperands() != 2)
            continue;

        Value *op1 = Inst.getOperand(0);
        Value *op2 = Inst.getOperand(1);

        auto BinOp = dyn_cast<BinaryOperator>(op1);
        auto C = dyn_cast<ConstantInt>(op2);

        if (!C || !BinOp)
            continue;
        outs()<<"Trovato const & op: "<<Inst<<"\n";

        auto BinOpConstant = BinOp->getOperand(1);

        if (BinOpConstant != C)
            continue;
        outs()<<"Possibile Eliminazione: "<<BinOpConstant<<" con "<<C<<"\n";

        auto Inst1OPC = Inst.getOpcode();
        auto Inst2OPC = BinOp->getOpcode();

        if( (Inst1OPC == Instruction::Add && Inst2OPC == Instruction::Sub) || (Inst1OPC == Instruction::Sub && Inst2OPC == Instruction::Add)){
            outs()<<"Operazioni opposte, Eliminazione...\n\n";
            Inst.replaceAllUsesWith(BinOp->getOperand(0));
            toDel.push_back(&Inst);
        }
    }

    for (auto i : toDel){
        i->eraseFromParent();
    }

    return true;
}

bool runOnFunctionMIP(Function &F) {
    bool Transformed = false;

    for (auto &Iter : F) {
      if (runOnBasicBlockMIP(Iter)) {
        Transformed = true;
      }
    }
  return Transformed;
};

PreservedAnalyses  MultiInstructionPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager   &) {
    for (auto &Iter : M) {
      if (runOnFunctionMIP(Iter)) {
        return PreservedAnalyses::none();
      }
    }

  return PreservedAnalyses::all();
}
