#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <math.h>
#include <list>

using namespace llvm;

bool runOnBasicBlockStrengthReduction(BasicBlock &BB)
{
    // Function *F = B.getParent();
    // LLVMContext &context = F->getContext();
    std::list<Instruction *> instructionsToBeRemove;

    for (auto &inst : BB)
    {

        outs() << inst << "\n";
        if (inst.isBinaryOp())
        {
            unsigned int opcode = inst.getOpcode();

            Value *opLeft = inst.getOperand(0);
            Value *opRight = inst.getOperand(1);

            ConstantInt *costantLeft = dyn_cast<ConstantInt>(opLeft);
            ConstantInt *costantRight = dyn_cast<ConstantInt>(opRight);

            Instruction *newInst = nullptr;

            switch (opcode)
            {
            case Instruction::Mul:
                outs() << "\tMoltiplicazione: \n";
                if (costantRight)
                {
                    if (costantRight->getValue().isPowerOf2())
                    {
                        APInt value = costantRight->getValue();
                        IntegerType *opType = costantRight->getType();

                        ConstantInt *numShift = ConstantInt::get(opType, value.exactLogBase2());
                        if (value.isNonNegative())
                        {
                            newInst = BinaryOperator::Create(Instruction::Shl, opLeft, numShift);
                            outs() << "\t Nuova istruzione" << *newInst << "\n";
                        }
                    }
                }
                break;

            case Instruction::SDiv:
            case Instruction::UDiv:
                outs() << "\tDivisione: \n";
                break;
            }
            if (newInst)
            {
                newInst->insertBefore(&inst);
                inst.replaceAllUsesWith(newInst);
                instructionsToBeRemove.push_back(&inst);
            }
        }
    }
    for (auto i : instructionsToBeRemove)
        i->eraseFromParent();

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
