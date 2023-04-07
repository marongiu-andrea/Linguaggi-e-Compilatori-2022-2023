#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/BasicBlock.h>

using namespace llvm;

/*
 * Function that tries to apply a multi-instruction optimization to an addition in input and returns true if it succeeds.
 * First the function tries to convert the operands of the addition into instructions. Then it checks the first operand to
 * see if it is a subtraction and if its second operand is equal to the second operand of the input instruction. If all the
 * conditions are met, the function replaces all the uses of the input instruction with the first operand of the subtraction.
 * Otherwise similar checks are repeated with the second operand of the input but this time it checks if the second operand
 * of the subtraction is equal to the first one of the addition and then it replaces all the uses with the first operand of the subtraction.
 */
bool multi_op_add(Instruction& inst)
{
    Instruction* op0 = dyn_cast<Instruction>(inst.getOperand(0));
    Instruction* op1 = dyn_cast<Instruction>(inst.getOperand(1));

    if (op0 && op0->getOpcode() == Instruction::Sub && op0->getOperand(1) == inst.getOperand(1))
    {
        inst.replaceAllUsesWith(op0->getOperand(0));
        return true;
    }

    if (op1 && op1->getOpcode() == Instruction::Sub && op1->getOperand(1) == inst.getOperand(0))
    {
        inst.replaceAllUsesWith(op1->getOperand(0));
        return true;
    }

    return false;
}

/*
 * Function that tries to apply a multi-instruction optimization to a subtraction in input and returns true if it succeeds.
 * First the function tries to convert the operands of the subtraction into instructions. Then it checks the first operand to
 * see if it is an addition and if any of its operands are equal to the second operand of the subtraction. If all the conditions
 * are met, the uses of the input instruction are replaced with the operand of the addition that is different from the second
 * operand of the subtraction. Otherwise similar checks are made but this time it checks if the second operand of the input is an
 * addition with any operand equal to the first operand of the input. In this case the input instruction is replaced with another
 * instruction that computes the opposite of the addition’s operand that is different from the first operand of the input.
 */
bool multi_op_sub(Instruction& inst)
{
    Instruction* op0 = dyn_cast<Instruction>(inst.getOperand(0));
    Instruction* op1 = dyn_cast<Instruction>(inst.getOperand(1));

    if (op0 && op0->getOpcode() == Instruction::Add)
    {

        if (op0->getOperand(0) == inst.getOperand(1))
        {
            inst.replaceAllUsesWith(op0->getOperand(1));
            return true;
        }

        if (op0->getOperand(1) == inst.getOperand(1))
        {
            inst.replaceAllUsesWith(op0->getOperand(0));
            return true;
        }
    }

    if (op1 && op1->getOpcode() == Instruction::Add)
    {

        if (op1->getOperand(0) == inst.getOperand(0))
        {
            llvm::Type* i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
            Instruction* newinst = BinaryOperator::Create(Instruction::Sub, ConstantInt::get(i64_type, 0), op1->getOperand(1));
            newinst->insertAfter(&inst);
            inst.replaceAllUsesWith(newinst);
            return true;
        }

        if (op1->getOperand(1) == inst.getOperand(0))
        {
            llvm::Type* i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
            Instruction* newinst = BinaryOperator::Create(Instruction::Sub, ConstantInt::get(i64_type, 0), op1->getOperand(0));
            newinst->insertAfter(&inst);
            inst.replaceAllUsesWith(newinst);
            return true;
        }
    }

    return false;
}

/*
 * Function that tries to apply a multi-instruction optimization to a multiplication in input and returns true if it succeeds.
 * First the function tries to convert the operands of the multiplication into instructions. Then it checks the first operand to
 * see if it is a division and if its second operand is equal to the second operand of the input instruction. If all the conditions
 * are met, the function replaces all the uses of the input instruction with the first operand of the division. Otherwise similar
 * checks are repeated with the second operand of the input but this time it checks if the second operand of the division is equal
 * to the first one of the multiplication and then it replaces all the uses with the first operand of the division.
 */
bool multi_op_mul(Instruction& inst)
{
    Instruction* op0 = dyn_cast<Instruction>(inst.getOperand(0));
    Instruction* op1 = dyn_cast<Instruction>(inst.getOperand(1));

    if (op0 && (op0->getOpcode() == Instruction::SDiv || op0->getOpcode() == Instruction::UDiv) && op0->getOperand(1) == inst.getOperand(1))
    {
        inst.replaceAllUsesWith(op0->getOperand(0));
        return true;
    }

    if (op1 && (op1->getOpcode() == Instruction::SDiv || op1->getOpcode() == Instruction::UDiv) && op1->getOperand(1) == inst.getOperand(0))
    {
        inst.replaceAllUsesWith(op1->getOperand(0));
        return true;
    }

    return false;
}

/*
 * Function that tries to apply a multi-instruction optimization to a division in input and returns true if it succeeds.
 * First the function tries to convert the operands of the division into instructions. Then it checks the first operand to
 * see if it is a multiplication and if any of its operands are equal to the second operand of the division. If all the
 * conditions are met, the uses of the input instruction are replaced with the operand of the multiplication that is different
 * from the second operand of the division. Otherwise similar checks are made but this time it checks if the second operand of
 * the input is a multiplication with any operand equal to the first operand of the input. In this case the input instruction is
 * replaced with another instruction that computes the inverse of the multiplication’s operand that is different from the first operand of the input.
 */
bool multi_op_div(Instruction& inst)
{
    Instruction* op0 = dyn_cast<Instruction>(inst.getOperand(0));
    Instruction* op1 = dyn_cast<Instruction>(inst.getOperand(1));

    if (op0 && op0->getOpcode() == Instruction::Mul)
    {

        if (op0->getOperand(0) == inst.getOperand(1))
        {
            inst.replaceAllUsesWith(op0->getOperand(1));
            return true;
        }

        if (op0->getOperand(1) == inst.getOperand(1))
        {
            inst.replaceAllUsesWith(op0->getOperand(0));
            return true;
        }
    }

    if (op1 && op1->getOpcode() == Instruction::Mul)
    {

        if (op1->getOperand(0) == inst.getOperand(0))
        {
            llvm::Type* i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
            Instruction* newinst = BinaryOperator::Create(Instruction::SDiv, ConstantInt::get(i64_type, 1), op1->getOperand(1));
            newinst->insertAfter(&inst);
            inst.replaceAllUsesWith(newinst);
            return true;
        }

        if (op1->getOperand(1) == inst.getOperand(0))
        {
            llvm::Type* i64_type = llvm::IntegerType::getInt64Ty(inst.getContext());
            Instruction* newinst = BinaryOperator::Create(Instruction::SDiv, ConstantInt::get(i64_type, 1), op1->getOperand(0));
            newinst->insertAfter(&inst);
            inst.replaceAllUsesWith(newinst);
            return true;
        }
    }

    return false;
}

/*
 * Simple function that iterates through the instructions of the input basic block and tries to apply
 * an optimization according to the type of the instruction. If an optimization is applied, the function
 * saves the affected instruction in a vector to later delete it. If at least an optimization was applied, the function returns true.
 */
bool MultiInstrOptPass::runOnBasicBlock(llvm::BasicBlock& bb)
{
    bool transformed = false;
    SmallVector<Instruction*, 0> to_delete;

    for (auto& inst : bb)
    {
        switch (inst.getOpcode())
        {
            case Instruction::Add:
                if (multi_op_add(inst))
                {
                    to_delete.push_back(&inst);
                    transformed = true;
                }
                break;

            case Instruction::Sub:
                if (multi_op_sub(inst))
                {
                    to_delete.push_back(&inst);
                    transformed = true;
                }
                break;

            case Instruction::Mul:
                if (multi_op_mul(inst))
                {
                    to_delete.push_back(&inst);
                    transformed = true;
                }
                break;

            case Instruction::SDiv:
                if (multi_op_div(inst))
                {
                    to_delete.push_back(&inst);
                    transformed = true;
                }
                break;

            case Instruction::UDiv:
                if (multi_op_add(inst))
                {
                    to_delete.push_back(&inst);
                    transformed = true;
                }
                break;
        }
    }

    for (auto* instr : to_delete)
    {
        instr->eraseFromParent();
    }

    return transformed;
}
