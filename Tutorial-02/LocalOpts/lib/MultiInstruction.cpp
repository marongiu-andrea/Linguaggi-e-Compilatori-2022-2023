#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <math.h>
#include <list>
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

bool runOnBasicBlockMultiInstructon(BasicBlock &BB)
{
    Function *F = BB.getParent();
    LLVMContext &context = F->getContext();
    llvm::IRBuilder<> builder(context);

    std::list<Instruction *> instructionsToBeRemoved;

    for (auto &inst : BB)
    {

        outs() << inst << "\n";
        if (inst.isBinaryOp())
        {
            unsigned int opcode = inst.getOpcode();

            Value *opLeft = inst.getOperand(0);
            Value *opRight = inst.getOperand(1);

            ConstantInt *constantLeft = dyn_cast<ConstantInt>(opLeft);
            ConstantInt *constantRight = dyn_cast<ConstantInt>(opRight);

            BinaryOperator *binOpLeft = dyn_cast<BinaryOperator>(opLeft);
            BinaryOperator *binOpRight = dyn_cast<BinaryOperator>(opRight);

            switch (opcode)
            {
            case Instruction::Add:
                outs() << "Add \n";

                if (binOpLeft && binOpLeft->getOpcode() == Instruction::Sub)
                {
                    Value *subOpRight = binOpLeft->getOperand(1);
                    if (subOpRight == opRight)
                    {

                        // inst.replaceAllUsesWith(binOpLeft->getOperand(0));

                        // elimina instr

                        outs() << "padre  " << val0 << " \n";
                    }
                }
                outs() << "op1: " << *opLeft << " \n";
                outs() << "op2: " << opRight->getValueID() << *opRight << " \n";

                break;

            case Instruction::Sub:
                outs() << "Sub \n";

                break;
            }
        }
    }
    for (auto i : instructionsToBeRemoved)
        i->eraseFromParent();

    return true;
}

bool runOnFunctionMultiInstructon(Function &F)
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter)
    {
        if (runOnBasicBlockMultiInstructon(*Iter))
        {
            Transformed = true;
        }
    }

    return Transformed;
}

PreservedAnalyses MultiInstructonPass::run([[maybe_unused]] Module &M,
                                           ModuleAnalysisManager &)
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter)
    {
        if (runOnFunctionMultiInstructon(*Iter))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}
