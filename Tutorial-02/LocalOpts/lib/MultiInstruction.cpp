#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <math.h>
#include <list>

using namespace llvm;

bool runOnBasicBlockMultiInstructon(BasicBlock &BB)
{
    Function *F = BB.getParent();
    LLVMContext &context = F->getContext();

    std::list<Instruction *> instructionsToBeRemoved;

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

            switch (opcode)
            {
            case Instruction::Add:
                outs() << "Add \n";
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
