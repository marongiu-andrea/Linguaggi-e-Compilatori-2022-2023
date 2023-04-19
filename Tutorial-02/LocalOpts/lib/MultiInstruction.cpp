#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;


bool runMIOnBasicBlock(BasicBlock &B) {

    for (auto Iter = ++B.begin(); Iter != B.end(); ++Iter){
        Instruction &Inst = *Iter;
        if (Inst.isBinaryOp()){
          switch (Inst.getOpcode()){

            case Instruction::Add:

                if (ConstantInt *C1 = dyn_cast<ConstantInt>(Inst.getOperand(0)))
                  if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(1)))
                    if (I->getOpcode() == Instruction::Sub)
                        if (ConstantInt *C2 = dyn_cast<ConstantInt>(I->getOperand(1)))
                           if (C1 == C2)
                            Inst.replaceAllUsesWith(I->getOperand(0));


                if (ConstantInt *C1 = dyn_cast<ConstantInt>(Inst.getOperand(1)))
                  if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(0)))
                    if (I->getOpcode() == Instruction::Sub)
                        if (ConstantInt *C2 = dyn_cast<ConstantInt>(I->getOperand(1)))
                          if (C1 == C2)
                            Inst.replaceAllUsesWith(I->getOperand(0));
                  
                break;

            case Instruction::Sub:

                if (ConstantInt *C1 = dyn_cast<ConstantInt>(Inst.getOperand(1)))
                  if (Instruction *I = dyn_cast<Instruction>(Inst.getOperand(0)))
                    if (I->getOpcode() == Instruction::Add){

                        if (ConstantInt *C2 = dyn_cast<ConstantInt>(I->getOperand(0)))
                            if(C1 == C2)
                                Inst.replaceAllUsesWith(I->getOperand(1));

                        if (ConstantInt *C2 = dyn_cast<ConstantInt>(I->getOperand(1)))
                            if(C1 == C2)
                                Inst.replaceAllUsesWith(I->getOperand(0));
                    }
          }
        }

    }
    
    return true;
  }


  bool runMIOnFunction(Function &F) {
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
      if (runMIOnBasicBlock(*Iter)) {
        Transformed = true;
      }
    }

    return Transformed;
  }




PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runMIOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

