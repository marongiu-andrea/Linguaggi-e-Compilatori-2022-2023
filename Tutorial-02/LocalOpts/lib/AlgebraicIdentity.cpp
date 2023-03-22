#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>

using namespace llvm;

bool runOnBasicBlockAlgebric(BasicBlock &B)
{
    Function *F = B.getParent();
    LLVMContext &context = F->getContext();

    int index = 1;
    Instruction *old_instr = nullptr;
    for (auto Iter = B.begin(); Iter != B.end(); ++Iter)
    {
        // Per ogni istruzione del BasicBlock

        Instruction &Inst = *Iter;

        if (!strcmp(Inst.getOpcodeName(), "add"))
        {
            // Se l'istruzione è una moltiplicazione o addizione

            outs() << Inst << "\n";
            Value *operand_1 = Inst.getOperand(0);
            Value *operand_2 = Inst.getOperand(1);
            outs() << "\t" << *operand_1 << "\n\t" << *operand_2 << "\n";

            ConstantInt *C2 = dyn_cast<ConstantInt>(operand_2);
            if (C2 && C2->getValue().isZero())
            {
                outs() << "Istruzione rimpiazzata con " << *operand_1 << "\n";
                Inst.replaceAllUsesWith(operand_1);
                // Inst.eraseFromParent();
            }
        }
        outs() << "\n\n";
        if (!strcmp(Inst.getOpcodeName(), "mul"))
        {
            // Se l'istruzione è una moltiplicazione o addizione

            int op_index = 0;

            outs() << Inst << "\n";
            Value *operand_1 = Inst.getOperand(0);
            Value *operand_2 = Inst.getOperand(1);
            outs() << "\t" << *operand_1 << "\n\t" << *operand_2 << "\n\n";

            ConstantInt *C1 = dyn_cast<ConstantInt>(operand_1);
            ConstantInt *C2 = dyn_cast<ConstantInt>(operand_2);
            if (C2)
            {
                APInt intVal = C2->getValue();

                int Val = static_cast<float>(intVal.getLimitedValue());
            }
        }
        old_instr = &(*Iter);
    }

    return true;
}

bool runOnFunctionAlgebric(Function &F)
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
    {
        if (runOnBasicBlockAlgebric(*Iter))
        {
            Transformed = true;
        }
    }

    return Transformed;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &)
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
    {
        if (runOnFunctionAlgebric(*Iter))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}
