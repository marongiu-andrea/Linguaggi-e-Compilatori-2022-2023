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
    //la funzione prende l'operando che punta all'istruzione, il valore della costante destra, l'istruzione, la lista delle istruzioni da elminare e il tipo di operazione
    BinaryOperator *binOpWithInstruction = dyn_cast<BinaryOperator>(opWithInstruction);

    if (binOpWithInstruction && binOpWithInstruction->getOpcode() == typeInstruction)
    {
        //se l'istruzione è una binary operator e il tipo di operazione è uguale a quello corrente prendiamo il 2 operando dell'istruzione
        Value *subOpConstant = binOpWithInstruction->getOperand(1);
        if (subOpConstant == opConstant)
        {
            //se la costante dell'istruzione di opWithInstruction + pari a quella corrente possiamo rimpiazzarle

            inst.replaceAllUsesWith(binOpWithInstruction->getOperand(0));
            instructionsToBeRemoved.push_back(&inst);

            outs() << "\tElimino istruzione e rimpiazzo con " << *(binOpWithInstruction->getOperand(0)) << " \n";
        }
    }
}

bool runOnBasicBlockMultiInstructon(BasicBlock &BB)
{
    //ottengo il contesto della funzione padre del basic block corrente
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
            // per ogni istruzione prendiamo i due operando e la tipologia di operazione
            switch (opcode)
            {
            case Instruction::Add:
                if (constantRight)
                    //se l'istruzione è una add e lìoperando a destra è una costante intera eseguiamo l'ottimizzazione
                    optimizeMultiInstruction(opLeft, opRight, inst, instructionsToBeRemoved, Instruction::Sub);
                break;

            case Instruction::Sub:
                if (constantRight)
                    //se l'istruzione è una sub e lìoperando a destra è una costante intera eseguiamo l'ottimizzazione
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
    //eseguiamo l'ottimizzazione su tutti i basic block delle funzioni del modulo
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
