#include "LocalOpts.h"
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instruction.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <vector>

using namespace llvm;

static bool isEligibleForAlgebraicIdentityOptimization(Instruction& instr, ConstantInt* constant)
{
    return ((instr.getOpcode() == Instruction::Add || instr.getOpcode() == Instruction::Sub) && constant->isZero()) ||
           ((instr.getOpcode() == Instruction::Mul || instr.getOpcode() == Instruction::SDiv || instr.getOpcode() == Instruction::UDiv) && constant->isOne());
}

static bool isOpcodeValidForAlgebraicIdentityOptimization(const Instruction& instr)
{
    return instr.getOpcode() == Instruction::Add ||
           instr.getOpcode() == Instruction::Sub ||
           instr.getOpcode() == Instruction::Mul ||
           instr.getOpcode() == Instruction::SDiv ||
           instr.getOpcode() == Instruction::UDiv;
}

bool AlgebraicIdentityPass::runOnBasicBlock(BasicBlock& bb)
{
    bool transformed = false;

    // salvo le istruzioni da rimuovere in questo vettore temporaneo perché non posso cancellarle mentre ci sto iterando sopra
    std::vector<std::pair<Instruction*, Value*>> instructionsToReplace;

    for (auto& instr : bb)
    {
        if (isOpcodeValidForAlgebraicIdentityOptimization(instr))
        {
            Value* left = instr.getOperand(0);
            Value* right = instr.getOperand(1);
            Value* operand = left;
            ConstantInt* constant = dyn_cast<ConstantInt>(right); // assumo che l'operando di destra sia una costante

            if (constant == nullptr && instr.isCommutative())
            {
                operand = right;
                constant = dyn_cast<ConstantInt>(left); // se l'operando di destra non è una costante, provo a verificare se lo è quello di sinistra
            }

            if (constant != nullptr) // l'algebraic identity si può fare solo se uno dei due operandi è una costante
            {
                if (isEligibleForAlgebraicIdentityOptimization(instr, constant))
                {
                    instructionsToReplace.emplace_back(&instr, operand);

                    transformed = true;
                }
            }
        }
    }

    for (auto& pair : instructionsToReplace)
    {
        Instruction* replacedInstr = pair.first;
        Value* replacingValue = pair.second;
        BasicBlock::iterator it(replacedInstr); // ReplaceInstWithValue vuole un iteratore di istruzioni da rimpiazzare

        ReplaceInstWithValue(bb.getInstList(), it, replacingValue);
    }

    return transformed;
}