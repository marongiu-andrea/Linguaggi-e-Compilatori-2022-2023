#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

//opt -load-pass-plugin=./libLocalOpts.so -passes=algebraicidentity test/test.ll -o test/test.algebraicidentity.optimized.bc
//llvm-dis test/test.algebraicidentity.optimized.bc -o test/test.algebraicidentity.optimized.ll

bool runOnBasicBlockAlgebraicIdentity(BasicBlock &B) 
{
    for (auto &Iter : B) // Itera sulle istruzioni del Basic Block
    {
        switch(Iter.getOpcode())
        {
            case Instruction::Add: //Se l'operazione è una Add
            {
                outs()<<"Ho trovato una Add\n";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (constant0 && (!constant1)) //Se solo il primo operando è una costante
                {
                    outs()<<"Ho trovato una costante in posizione 0\n";
                    if ((*constant0).isZeroValue()) // Se il primo operando è 0
                    {
                        outs()<<"L'operatore in posizione 0 vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(1));
                    }                    
                }
                if (constant1 && (!constant0)) //Se solo il secondo operando è una costante 
                {
                    outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
                    if ((*constant1).isZeroValue()) // Se il secondo operando è 0
                    {
                        outs()<<"L'operatore in posizione 1 vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(0));
                    }
                }

                break;
            }
            case Instruction::Sub: //Se l'operazione è una Sub
            {
                outs()<<"Ho trovato una Sub\n";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (constant1 && (!constant0)) //Se solo il secondo operando è una costante 
                {
                    outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
                    if ((*constant1).isZeroValue()) // Se il secondo operando è 0
                    {
                        outs()<<"L'operatore in posizione 1 vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(0));
                    }
                }

                break;
            }
            case Instruction::Mul: //Se l'operazione è una Mul
            {
                outs()<<"Ho trovato una Mul\n";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (constant0 && (!constant1)) //Se solo il primo operando è una costante
                {
                    outs()<<"Ho trovato una costante in posizione 0\n";
                    if ((*constant0).isOneValue()) // Se il primo operando è 1
                    {
                        outs()<<"L'operatore in posizione 0 vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(1));
                    }                    
                }
                if (constant1 && (!constant0)) //Se solo il secondo operando è una costante 
                {
                    outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*constant1).getValue()<<"\n";
                    if ((*constant1).isOneValue()) // Se il secondo operando è 1
                    {
                        outs()<<"L'operatore in posizione 1 vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(0));
                    }
                }

                break;
            }
            case Instruction::SDiv: //Se l'operazione è una SDiv
            {
                outs()<<"Ho trovato una SDiv ";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (constant1 && (!constant0)) //Se solo il secondo operando è una costante 
                {
                    outs()<<"con una costante in posizione 1 di valore "<<(*constant1).getValue()<<" ";
                    if ((*constant1).isOneValue()) // Se il secondo operando è 1
                    {
                        outs()<<"che vale 0\n";
                        Iter.replaceAllUsesWith(Iter.getOperand(0));
                    }
                }

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

