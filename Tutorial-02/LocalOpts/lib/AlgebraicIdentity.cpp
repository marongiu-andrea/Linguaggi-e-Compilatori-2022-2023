#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

// Algebraic Identity:
//  𝑥 + 0 = 0 + 𝑥 ⇒ 𝑥
//  𝑥 × 1 = 1 × 𝑥 ⇒ 𝑥

//opt -load-pass-plugin=./libLocalOpts.so -passes=algebraicidentity test/test.ll -o test/test.algebraicidentity.optimized.bc
//llvm-dis test/test.algebraicidentity.optimized.bc -o test/test.algebraicidentity.optimized.ll

void addAlgebraicIdentity(Instruction &Iter) 
{

    outs()<<"----------------------------------------------------------------------------\nHo trovato una Add:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (constant0 && (!constant1)) //Se solo il primo operando è una costante
    {
        outs()<<"Ho trovato una costante in posizione 0 di valore "<<(*constant0).getValue()<<"\n";
        if ((*constant0).isZeroValue()) // Se il primo operando è 0
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 1\n";
            Iter.replaceAllUsesWith(Iter.getOperand(1));
        }                    
    }
    if ((!constant0) && constant1) //Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
        if ((*constant1).isZeroValue()) // Se il secondo operando è 0
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 0\n";
            Iter.replaceAllUsesWith(Iter.getOperand(0));
        }
    }
}

void subAlgebraicIdentity(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Sub:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if ((!constant0) && constant1) //Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
        if ((*constant1).isZeroValue()) // Se il secondo operando è 0
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 0\n";
            Iter.replaceAllUsesWith(Iter.getOperand(0));
        }
    }
}

void mulAlgebraicIdentity(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Mul:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (constant0 && (!constant1)) //Se solo il primo operando è una costante
    {
        outs()<<"Ho trovato una costante in posizione 0 di valore "<<(*constant0).getValue()<<"\n";
        if ((*constant0).isOneValue()) // Se il primo operando è 1
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 1\n";
            Iter.replaceAllUsesWith(Iter.getOperand(1));
        }                    
    }
    if ((!constant0) && constant1) //Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
        if ((*constant1).isOneValue()) // Se il secondo operando è 1
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 0\n";
            Iter.replaceAllUsesWith(Iter.getOperand(0));
        }
    }
}

void sdivAlgebraicIdentity(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una SDiv:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (constant1 && (!constant0)) //Se solo il secondo operando è una costante 
    {
        outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
        if ((*constant1).isOneValue()) // Se il secondo operando è 1
        {
            outs()<<" --> Rimpiazzo tutti gli usi dell'istruzione con l'operatore in posizione 0\n";
            Iter.replaceAllUsesWith(Iter.getOperand(0));
        }
    }
}

bool runOnBasicBlockAlgebraicIdentity(BasicBlock &B) 
{
    for (Instruction &Iter : B) // Itera sulle istruzioni del Basic Block
    {
        switch(Iter.getOpcode())
        {
            case Instruction::Add: //Se l'operazione è una Add
            {
                addAlgebraicIdentity(Iter);
                break;
            }
            case Instruction::Sub: //Se l'operazione è una Sub
            {
                subAlgebraicIdentity(Iter);
                break;
            }
            case Instruction::Mul: //Se l'operazione è una Mul
            {
                mulAlgebraicIdentity(Iter);
                break;
            }
            case Instruction::SDiv: //Se l'operazione è una SDiv
            {
                sdivAlgebraicIdentity(Iter);
                break;
            }   
            default:
            {
                break;
            }           
        }
    }
    return true;    
}

bool runOnFunctionAlgebraicIdentity(Function &F) 
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
        if (runOnBasicBlockAlgebraicIdentity(*Iter)) 
            Transformed = true;
    }
    return Transformed;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunctionAlgebraicIdentity(*Iter)) 
            return PreservedAnalyses::none();
    }

  return PreservedAnalyses::none();
}

