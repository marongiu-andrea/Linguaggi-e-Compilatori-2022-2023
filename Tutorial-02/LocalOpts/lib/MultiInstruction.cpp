#include "LocalOpts.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include <vector>

using namespace llvm;

static std::vector<Instruction *> removableInstructions;

static bool areOperationCodesOpposite(unsigned int opcode1,
                                      unsigned int opcode2) {
  if (opcode1 == Instruction::Add && opcode2 == Instruction::Sub) {
    return true;
  } else if (opcode1 == Instruction::Sub && opcode2 == Instruction::Add) {
    return true;
  }

  return false;
}

static bool runOnBasicBlock4(BasicBlock &B) {
  for (auto Iter = B.begin(); Iter != B.end(); ++Iter) {
    Instruction &Inst = *Iter;

    if (Inst.getNumOperands() != 2) {
      continue;
    }

    Value *leftOperand = Inst.getOperand(0);
    Value *rightOperand = Inst.getOperand(1);

    /*
        The register of the 'leftOperand' might contains a bynary operation, eg:
            a = 5 + x
            b = a + 1  -> the register that contains 'a' contains '5+x' that is
       a binary operation
    */
    auto binaryOperation = dyn_cast<BinaryOperator>(leftOperand);
    auto constant = dyn_cast<ConstantInt>(rightOperand);

    if (!constant || !binaryOperation) {
      continue;
    }

    auto binaryOperationConstant = binaryOperation->getOperand(1);

    if (binaryOperationConstant != constant) {
      continue;
    }

    auto operationCode = Inst.getOpcode();
    auto binaryOperationOpcode = binaryOperation->getOpcode();

    if (areOperationCodesOpposite(operationCode, binaryOperationOpcode)) {
      Inst.replaceAllUsesWith(binaryOperation->getOperand(0));
      removableInstructions.push_back(&Inst);
    }
  }

  for (auto removableInstruction : removableInstructions) {
    removableInstruction->eraseFromParent();
  }

  return true;
}

bool runOnFunction4(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock4(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,
                                            ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction4(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}