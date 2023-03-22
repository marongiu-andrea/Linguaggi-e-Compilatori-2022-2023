#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

//opt -load-pass-plugin=./libLocalOpts.so -passes=algebraicidentity test/test.ll -o test/test.algebraicidentity.optimized.bc

bool runOnBasicBlockAlgebraicIdentity(BasicBlock &B) 
{
    for (auto &Iter : B) // Itera sulle istruzioni del Basic Block
    {

        if (Iter.getOpcode() == Instruction::Add)
        {
            outs()<<"Ho trovato una Add\n";
            if (ConstantInt * constant = dyn_cast<ConstantInt>(Iter.getOperand(0)))
            {
                outs()<<"Ho trovato una costante in posizione 0\n";
            }
            if (ConstantInt * constant = dyn_cast<ConstantInt>(Iter.getOperand(1)))
            {
                outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant).getValue()<<"\n";
                if ((*constant).isZeroValue())
                {
                    outs()<<"L'operatore in posizione 1 vale 0\n";
                }
            }

        }
        if (Iter.getOpcode() == Instruction::Sub)
            outs()<<"Ho trovato una Sub\n";
        if (Iter.getOpcode() == Instruction::Mul)
            outs()<<"Ho trovato una Mul\n";
        if (Iter.getOpcode() == Instruction::SDiv)
            outs()<<"Ho trovato una SDiv\n";
    }

    return true;    
}


bool runOnFunctionAlgebraicIdentity(Function &F) 
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
        if (runOnBasicBlockAlgebraicIdentity(*Iter)) 
        {
            Transformed = true;
        }
    }

    return Transformed;
}




PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunctionAlgebraicIdentity(*Iter)) 
        {
            return PreservedAnalyses::none();
        }
    }

  return PreservedAnalyses::none();
}

