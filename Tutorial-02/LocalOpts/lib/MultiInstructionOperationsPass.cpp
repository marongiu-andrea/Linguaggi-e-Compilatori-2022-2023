#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
using namespace llvm;

/**
 * controls left operand of instruction passed and replace usee of passed instruction with operand 1 of usee operands if both instructions
 * have same constant but different operation 
*/
static bool checkUseeInstruction(Value *opL, Value *opR, Instruction &inst, unsigned int opcode){
  BinaryOperator *opLbinaryIns = dyn_cast<BinaryOperator>(opL);
  if (opLbinaryIns) {
    unsigned int opcodeopL = opLbinaryIns->getOpcode();

    if ((opcode == Instruction::Add) && (opcodeopL == Instruction::Sub)) {
      if (opR == dyn_cast<Value>(opLbinaryIns->getOperand(1))) {
        inst.replaceAllUsesWith(opLbinaryIns->getOperand(0));
        
        return true;
      }
    }
    else if ((opcode == Instruction::Sub) && (opcodeopL == Instruction::Add)) {
      if (opR == dyn_cast<Value>(opLbinaryIns->getOperand(1))) {
        inst.replaceAllUsesWith(opLbinaryIns->getOperand(0));

        return true;
      }
    }
  }

  return false;
}

/**
 * this function performs iteration on instructions of basic block, checks instruction if it is addition or subtraction
 *  
*/
static bool runOnBasicBlock(BasicBlock &B){
  std::vector<Instruction*> instRemove;

  for (auto &inst : B) {
    if (inst.isBinaryOp()) {
      bool remove_inst = false;
      Value *opL = inst.getOperand(0);
      Value *opR = inst.getOperand(1);
      unsigned int opcode = inst.getOpcode();

      if (opcode == Instruction::Add) {
        remove_inst = checkUseeInstruction(opL, opR, inst, opcode);
      } else if (opcode == Instruction::Sub) {
        remove_inst = checkUseeInstruction(opL, opR, inst, opcode);
      }

      if (remove_inst) {
        instRemove.push_back(&inst);
      }
    }
  }

  for (auto ins : instRemove) {
    ins->eraseFromParent();
  }

  return true;
}

static bool runOnFunction(Function &F){
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses MultiInstructionOperationsPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  outs() << "MultiInstructionOperations\n";

  bool transformed = false;
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    transformed |= runOnFunction(*Iter);
  }

  if (transformed) {
    return PreservedAnalyses::none();
  } else {
    return PreservedAnalyses::all();
  }
}