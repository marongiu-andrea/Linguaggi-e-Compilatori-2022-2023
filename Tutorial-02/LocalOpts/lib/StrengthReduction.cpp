#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include <string.h>
#include <math.h>
#include <list>

using namespace llvm;

std::list<Instruction *> instructionsToBeRemoved;

void check_and_create(ConstantInt *operand, Value *operand_to_shift,std::string position, LLVMContext &context, Instruction &inst)
{
    
    Instruction *newShiftInst = nullptr;
    Instruction *newSubInst = nullptr;
    APInt value = operand->getValue();

    if (value.isPowerOf2())
    {

        
        IntegerType *opType = operand->getType();

        ConstantInt *numShift = ConstantInt::get(opType, value.exactLogBase2());
        if (value.isNonNegative())
        {
            //questo si fa nel caso abbiamo una potenza esatta di 2 e quindi non serve la sub
            newShiftInst = BinaryOperator::Create(Instruction::Shl, operand_to_shift, numShift);
            outs() << "\t Nuova istruzione di shift a sinistra di passi: " << numShift->getValue() << " sono a " << position << "\n";

        }
    }
    else
    {
        //nel caso non sia una potenza esatta di 2, bisogna trovare quella più vicina che sia maggiore
        int value_int = value.getLimitedValue();
        double log_value = log2(static_cast<double>(value_int));
        double shift_len = ceil(log_value);
        //prendo il valore intero dall'oggetto APInt e faccio la log2,dopo calcolo il ceil del valore ottenuto
        //questo per avere sempre la prima potenza di 2 maggiore del valore
        //esempio 15 avrà 4 e 17 avrà 5

        outs() << "\tThe number of shifts of value " << value_int << " is " << shift_len << "\n";
        //int power_of_log = pow(2,static_cast<int>(shift_len));
        int shift_len_int = static_cast<int>(shift_len);
       

        //costanti intere per la shift e la sub
        ConstantInt *api_shift = ConstantInt::get(context, APInt(32, shift_len_int));
        ConstantInt *api_sub = ConstantInt::get(context, APInt(32, value_int));

        newShiftInst = BinaryOperator::Create(Instruction::Shl, operand_to_shift, api_shift);
        newSubInst = BinaryOperator::Create(Instruction::Sub, newShiftInst, api_sub);
    }

    //la shift c'è sempre siccome siamo in una mul
    newShiftInst->insertAfter(&inst);

    //la sub bisogna vedere se è stata usata quindi non è null
    if(newSubInst)
    {
        newSubInst->insertAfter(newShiftInst);
    }

    //nel caso non ci sia una sub rimpiazzo gli usi con la nuova istruzione di shift altrimenti con quella di sub
    if(newShiftInst &&  !newSubInst)
    {
        inst.replaceAllUsesWith(newShiftInst);
    }
    else
    {
        inst.replaceAllUsesWith(newSubInst);
    }

    //aggiungo l'istruzione corrente a quelle da elminare
    instructionsToBeRemoved.push_back(&inst);
    
}

bool runOnBasicBlockStrengthReduction(BasicBlock &BB)
{
    Function *F = BB.getParent();
    LLVMContext &context = F->getContext();
    

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
            case Instruction::Mul:
                outs() << "\tMoltiplicazione: \n";
                if (costantRight)
                {   
                    check_and_create(costantRight,opLeft,"destra",context,inst);
                } 
                else if (costantLeft)
                {
                    check_and_create(costantLeft,opRight,"sinistra",context,inst);
                }
                break;

            case Instruction::SDiv:
                outs() << "\tDivisione: \n";

                break;
            }
            
        }
    }
    for (auto i : instructionsToBeRemoved)
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
