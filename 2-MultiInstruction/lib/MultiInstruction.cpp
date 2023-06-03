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

void optimizeMultiInstruction(Value *opWithInstruction, Value *opConstant, Instruction &inst, std::list<Instruction *> &instructionsToBeRemoved, unsigned typeInstruction)
{
    BinaryOperator *binOpWithInstruction = dyn_cast<BinaryOperator>(opWithInstruction);

    if (binOpWithInstruction && binOpWithInstruction->getOpcode() == typeInstruction)
    {
        Value *subOpConstant = binOpWithInstruction->getOperand(1);
        if (subOpConstant == opConstant)
        {

            inst.replaceAllUsesWith(binOpWithInstruction->getOperand(0));
            instructionsToBeRemoved.push_back(&inst);

            outs() << "\tElimino istruzione e rimpiazzo con " << *(binOpWithInstruction->getOperand(0)) << " \n";
        }
    }
}

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

            ConstantInt *constantRight = dyn_cast<ConstantInt>(opRight);

            switch (opcode)
            {
            case Instruction::Add:
                if (constantRight)
                    optimizeMultiInstruction(opLeft, opRight, inst, instructionsToBeRemoved, Instruction::Sub);
                break;

            case Instruction::Sub:
                if (constantRight)
                    optimizeMultiInstruction(opLeft, opRight, inst, instructionsToBeRemoved, Instruction::Add);
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
