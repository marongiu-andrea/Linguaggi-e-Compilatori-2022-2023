#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <cmath>
#include <stdint.h>

using namespace llvm;

static bool mulStrengthReductionNeeded(llvm::ConstantInt * op_const)
{
    if (!op_const || op_const->isNegative())
        return false;
    
    uint64_t intValue = op_const->getZExtValue();

    return isPowerOf2_64(intValue) || (intValue >= 1 && isPowerOf2_64(intValue - 1)) || isPowerOf2_64(intValue + 1);
}

static Instruction *mulStrengthReduction(Instruction *instr, llvm::Value *lhs, llvm::ConstantInt *rhs)
{
    if (!lhs || !mulStrengthReductionNeeded(rhs))
        return nullptr;

    uint64_t const_rhs = rhs->getZExtValue();
    uint64_t opType;

    if (isPowerOf2_64(const_rhs)) {
        opType = 0;
    } else if (const_rhs >= 1 && isPowerOf2_64(const_rhs - 1)) {
        const_rhs--;
        opType = Instruction::Add;
    } else if (isPowerOf2_64(const_rhs + 1)) {
        const_rhs++;
        opType = Instruction::Sub;
    } else {
        return nullptr;
    }
    
    Constant *rhsValue = ConstantInt::getSigned(rhs->getType(), log2(const_rhs));
    Instruction *sinstr = BinaryOperator::Create(Instruction::Shl, lhs, rhsValue);
    sinstr->insertAfter(instr);

    if (opType == 0) {
        instr->replaceAllUsesWith(sinstr);
        return sinstr;
    }
    
    Instruction *nextInstr = BinaryOperator::Create(static_cast<Instruction::BinaryOps>(opType), 
                                                    sinstr, lhs);
    nextInstr->insertAfter(sinstr);
    instr->replaceAllUsesWith(nextInstr);
    
    return sinstr;
}

static bool runOnInstruction(Instruction &instr)
{
    if (!instr.isBinaryOp())
        return false;

    auto operand1 = instr.getOperand(0);
    auto operand2 = instr.getOperand(1);
    ConstantInt* op1const = dyn_cast<ConstantInt>(operand1);
    ConstantInt* op2const = dyn_cast<ConstantInt>(operand2);
    Instruction* newInstr = nullptr;

    switch (instr.getOpcode())
    {
    case Instruction::Mul:
        newInstr = mulStrengthReduction(&instr, operand2, op1const);
        if (!newInstr) 
            newInstr = mulStrengthReduction(&instr, operand1, op2const);

        break;

    case Instruction::SDiv:
        if (op2const && !op2const->isNegative() && isPowerOf2_64(op2const->getZExtValue())) {
            Constant * rhsValue = ConstantInt::getSigned(operand2->getType(), log2(op2const->getZExtValue()));
            newInstr = BinaryOperator::Create(Instruction::LShr, operand1, rhsValue);

            newInstr->insertAfter(&instr);
            instr.replaceAllUsesWith(newInstr);
        }

        break;

    default:
        break;
    }

    return newInstr;
}

static bool runOnBasicBlock(BasicBlock &block) 
{
    std::vector<Instruction *> toRemove;
    for (auto &instr : block) {
        if (runOnInstruction(instr))
            toRemove.push_back(&instr);
    }

    for (auto instr: toRemove)
        instr->eraseFromParent();

    return !toRemove.empty();
}

static bool runOnFunction(Function &func) 
{
    bool transformed = false;

    for (auto &block : func) {
        if (runOnBasicBlock(block))
            transformed = true;
    }

    return transformed;
} 

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                            ModuleAnalysisManager &)
{
    for (auto &func : M) {
        if (runOnFunction(func))
            return PreservedAnalyses::none();
    }

    return PreservedAnalyses::none();                                  
}