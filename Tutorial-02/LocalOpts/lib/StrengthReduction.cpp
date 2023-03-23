#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <cmath>
#include <list>

using namespace llvm;

bool runOnBasicBlockStrengthReduction(BasicBlock &B)
{
    Function *F = B.getParent();
    LLVMContext &context = F->getContext();
    std::list<Instruction *> instructionToRemove;

    for (auto Iter = B.begin(); Iter != B.end(); ++Iter)
    {
        // Per ogni istruzione del BasicBlock
        Instruction &Inst = *Iter;
        outs() << Inst << "\n";
        Value *operand_1;
        Value *operand_2;

        switch (Inst.getOpcode())
        {
        case Instruction::Mul:
            outs() << "Moltiplicazione \n";
            operand_1 = Inst.getOperand(0);
            operand_2 = Inst.getOperand(1);

            // Controllo se il secondo operando è costante
            ConstantInt *C2 = dyn_cast<ConstantInt>(operand_2);

            if (C2)
            {
                if (C2->getValue().isPowerOf2())
                {

                    Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, operand_1, C2);
                    outs() << "\tNuova istruzione" << *NewInst << "\n";
                    NewInst->insertBefore(&Inst);
                    Inst.replaceAllUsesWith(NewInst);
                    instructionToRemove.push_back(&Inst);
                }
                // else if ((C2->getValue() + 1) % 2 == 0)
                // {
                // }
                // else if ((C2->getValue() - 1) % 2 == 0)
                // {
                // }
            }
            outs() << "\t" << *operand_1 << "\n\t" << *operand_2 << "\n";
            for (auto i : instructionToRemove)
                i->eraseFromParent();
            break;

            // case Instruction::SDiv:

            //     break;
        }

        outs() << "\n";
    }

    return true;
}

bool runOnFunctionStrengthReduction(Function &F)
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
    {
        if (runOnBasicBlockStrengthReduction(*Iter))
        {
            Transformed = true;
        }
    }

    return Transformed;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &)
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
    {
        if (runOnFunctionStrengthReduction(*Iter))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}
