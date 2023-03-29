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
           ((instr.getOpcode() == Instruction::Mul || instr.getOpcode() == Instruction::SDiv) && constant->isOne());
}

bool AlgebraicIdentityPass::runOnBasicBlock(BasicBlock& bb)
{
    bool transformed = false;

    // salvo le istruzioni da rimuovere in questo vettore temporaneo perché non posso cancellarle mentre ci sto iterando sopra
    std::vector<std::pair<Instruction*, Value*>> instructionsToReplace;

    auto bbBinaryInstructions = make_filter_range(bb, [](Instruction& instr) {
        return instr.isBinaryOp();
    });

    for (auto& instr : bbBinaryInstructions)
    {
        Value* left = instr.getOperand(0);
        Value* right = instr.getOperand(1);

        // di default assumo che l'operando di sinistra sia una variabile e quello di destra una costante
        Value* variable = left;
        ConstantInt* constant = dyn_cast<ConstantInt>(right);

        if (constant == nullptr && instr.isCommutative())
        {
            // se l'istruzione è commutativa e l'assunzione di prima era sbagliata, provo a controllare se i ruoli sono invertiti
            // (a sinistra la costante e a destra la variabile)
            variable = right;
            constant = dyn_cast<ConstantInt>(left);
        }

        if (constant != nullptr && dyn_cast<ConstantInt>(variable) == nullptr)
        {
            // operazione tra una variabile ed una costante - può essere una candidata per l'ottimizzazione

            if (isEligibleForAlgebraicIdentityOptimization(instr, constant))
            {
                // ho trovato un'istruzione da rimuovere

                instructionsToReplace.push_back(std::pair(&instr, variable));

                transformed = true;
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