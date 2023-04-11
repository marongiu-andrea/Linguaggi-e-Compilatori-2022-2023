#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <cmath>
#include <stdint.h>
#include <vector>

using namespace llvm;


static bool runOnInstruction(Instruction *instr) 
{
    if (!instr || !instr->isBinaryOp())
        return false;

    auto operand1 = instr->getOperand(0);
    auto operand2 = instr->getOperand(1);
    ConstantInt* op1const = dyn_cast<ConstantInt>(operand1);
    ConstantInt* op2const = dyn_cast<ConstantInt>(operand2);
    llvm::Value* newValue = nullptr;

    switch (instr->getOpcode())
    {
    case Instruction::Add:
    case Instruction::Sub:
        if (op1const && op1const->getSExtValue() == 0)
            newValue = operand2;
        else if (op2const && op2const->getSExtValue() == 0)
            newValue = operand1;

        break;
    
    case Instruction::Mul:
        if (op1const && op1const->getSExtValue() == 1)
            newValue = operand2;
        else if (op2const && op2const->getSExtValue() == 1)
            newValue = operand1;

        break;

    case Instruction::SDiv:
        if (op2const && op2const->getSExtValue() == 1)
            newValue = operand1;
        break;

    default:
        break;
    }

    if (!newValue)
        return false;

    instr->replaceAllUsesWith(newValue);

    return true;
}

static bool runOnBlock(BasicBlock& block)
{
    std::vector<Instruction *> toRemove;
    for (auto &instr : block) {
        if (runOnInstruction(&instr))
            toRemove.push_back(&instr);
    }

    for (auto *instr: toRemove)
        instr->eraseFromParent();

    return !toRemove.empty();
}

static bool runOnFunction(Function & func) 
{
    for (auto &block : func) {
        if (runOnBlock(block))
            return true;
    }

    return false;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                            ModuleAnalysisManager &) 
{

    for (auto &func: M) {
        if (runOnFunction(func))
            return PreservedAnalyses::none();
    }

    return PreservedAnalyses::none();
}