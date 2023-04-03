#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;


/**
 * @note Assumes the instruction is a binary operation.
 * @returns If an operand of the instruction is of type ValueType and matches the predicate,
 *          it returns the other operand;
 *          otherwise, it returns null.
*/
template <typename ValueType>
static Value* get_other_operand_if_one_is(Instruction& instruction, const std::function<bool(ValueType*)> predicate) {
  Value* op0 = instruction.getOperand(0);
  Value* op1 = instruction.getOperand(1);

  if (auto* val0 = dyn_cast<ValueType>(op0))
    if (predicate(val0))
      return op1;
  
  if (auto* val1 = dyn_cast<ValueType>(op1))
    if (predicate(val1))
      return op0;
    
  return nullptr;
}

/**
 * @note Assumes the instruction is a binary operation.
 * @returns If the second operand of the instruction is of type ValueType and matches the predicate,
 *          it returns the first operand;
 *          otherwise, it returns null.
*/
template <typename ValueType>
static Value* get_first_operand_if_second_is(Instruction& instruction, const std::function<bool(ValueType*)> predicate) {
  Value* op1 = instruction.getOperand(1);

  if (auto* val1 = dyn_cast<ValueType>(op1))
    if (predicate(val1))
      return instruction.getOperand(0);
    
  return nullptr;
}

/**
 * @returns If the instruction has to be deleted, returns the value which should replace all of its uses.
 *          Otherwise, it returns a null pointer.
*/
static Value* shouldBeReplacedWith(Instruction& inst) {
  switch (inst.getOpcode()) {
    // With commutativity

    case Instruction::Add:  // Integer add
    case Instruction::Sub:  // Integer sub
      return get_other_operand_if_one_is<ConstantInt>(inst, &ConstantInt::isZero);

    case Instruction::Mul:  // Integer multiplication
      return get_other_operand_if_one_is<ConstantInt>(inst, &ConstantInt::isOne);
    
    case Instruction::FAdd: // Floating point add
    case Instruction::FSub: // Floating point sub
      return get_other_operand_if_one_is<ConstantFP>(inst, &ConstantFP::isZero);


    // Without commutativity

    case Instruction::SDiv: // Signed division
    case Instruction::UDiv: // Unsigned division
      return get_first_operand_if_second_is<ConstantInt>(inst, &ConstantInt::isOne);

    case Instruction::Shl:  // Logical shift left
    case Instruction::LShr: // Logical shift right
    case Instruction::AShr: // Arithmetic shift right
      return get_first_operand_if_second_is<ConstantInt>(inst, &ConstantInt::isZero);

    default:
      return nullptr;
  }
}

static void runOnBasicBlock(BasicBlock& bb) {
  auto ii = bb.begin();

  while (ii != bb.end()) {
    Instruction& inst = *ii;

    if (Value* replace_with = shouldBeReplacedWith(inst)) {
      inst.replaceAllUsesWith(replace_with);

      // We delete an instruction, so the current iterator will not be valid anymore.
      // Fortunately EraseFromParent returns a valid iterator starting at the next instruction.
      ii = inst.eraseFromParent(); 

    } else {
      // We didn't replace anything, so we can go forward like usual.
      ++ii;
    }
  }
}

PreservedAnalyses AlgebraicIdentityPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  outs() << "AlgebraicIdentity\n";

  for (auto bb = F.begin(); bb != F.end(); ++bb)
    runOnBasicBlock(*bb);

  return PreservedAnalyses::none();
}