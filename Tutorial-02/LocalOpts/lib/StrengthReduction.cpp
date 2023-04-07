#include "LocalOpts.h"
#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instruction.h>
#include <llvm/Support/Casting.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

using namespace llvm;

static bool isOpcodeValidForStrengthReductionOptimization(const Instruction& instr)
{
    return instr.getOpcode() == Instruction::Mul ||
           instr.getOpcode() == Instruction::SDiv ||
           instr.getOpcode() == Instruction::UDiv;
}

bool StrengthReductionPass::runOnBasicBlock(BasicBlock& bb)
{
    std::vector<std::pair<Instruction*, Value*>> instructionsToReplace;

    for (auto& instr : bb)
    {
        // cerco le istruzioni di moltiplicazione
        if (isOpcodeValidForStrengthReductionOptimization(instr))
        {
            Value* left = instr.getOperand(0);
            Value* right = instr.getOperand(1);
            Value* operand = left;
            ConstantInt* constant = dyn_cast<ConstantInt>(right);

            if (constant == nullptr && instr.isCommutative())
            {
                operand = right;
                constant = dyn_cast<ConstantInt>(left);
            }

            if (constant != nullptr)
            {
                APInt constantValue = constant->getValue();

                if (constantValue.isPowerOf2())
                {
                    // moltiplicazione/divisione con una potenza di 2, si può applicare la strength reduction optimization

                    IRBuilder builder(&instr); // il builder aggiungerà le nuove istruzioni dopo quella corrente
                    ConstantInt* shiftFactor = ConstantInt::get(constant->getType(), constantValue.logBase2());
                    Value* shiftInstr = nullptr;

                    if (instr.getOpcode() == Instruction::Mul)
                    {
                        // sostituisco la moltiplicazione con un left shift

                        shiftInstr = builder.CreateShl(operand, shiftFactor, "shl", instr.hasNoUnsignedWrap(), instr.hasNoSignedWrap());
                    }
                    else
                    {
                        // sostituisco la divisione con un right shift aritmetico (i.e. tiene conto del segno)

                        shiftInstr = builder.CreateAShr(operand, shiftFactor);
                    }

                    instructionsToReplace.emplace_back(&instr, shiftInstr);
                }
                else if (instr.getOpcode() == Instruction::Mul)
                {
                    // se la costante non è una potenza di 2, ma ci si avvicina sufficientemente, si può comunque applicare la strength reduction

                    unsigned int constantValueNearestLogBase2 = constantValue.nearestLogBase2();

                    if (constantValue == (1 << constantValueNearestLogBase2) + 1)
                    {
                        // sostituisco la moltiplicazione con un left shift ed una add

                        IRBuilder builder(&instr);
                        ConstantInt* shiftFactor = ConstantInt::get(constant->getType(), constantValueNearestLogBase2);
                        Value* shiftInstr = builder.CreateShl(operand, shiftFactor, "shl", instr.hasNoUnsignedWrap(), instr.hasNoSignedWrap());
                        Value* addInstr = builder.CreateAdd(shiftInstr, operand, "add", instr.hasNoUnsignedWrap(), instr.hasNoSignedWrap());

                        instructionsToReplace.emplace_back(&instr, addInstr);
                    }
                    else if (constantValue == (1 << constantValueNearestLogBase2) - 1)
                    {
                        // sostituisco la moltiplicazione con un left shift ed una sub

                        IRBuilder builder(&instr);
                        ConstantInt* shiftFactor = ConstantInt::get(constant->getType(), constantValueNearestLogBase2);
                        Value* shiftInstr = builder.CreateShl(operand, shiftFactor, "shl", instr.hasNoUnsignedWrap(), instr.hasNoSignedWrap());
                        Value* subInstr = builder.CreateSub(shiftInstr, operand, "sub", instr.hasNoUnsignedWrap(), instr.hasNoSignedWrap());

                        instructionsToReplace.emplace_back(&instr, subInstr);
                    }
                }
            }
        }
    }

    for (auto& pair : instructionsToReplace)
    {
        BasicBlock::iterator it(pair.first);

        ReplaceInstWithValue(bb.getInstList(), it, pair.second);
    }

    return true;
}
