#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

//opt -load-pass-plugin=./libLocalOpts.so -passes=multiinstructionoptimization test/testMultiInstruction.ll -o test/test.multiinstructionoptimization.optimized.bc
//llvm-dis test/test.multiinstructionoptimization.optimized.bc -o test/test.multiinstructionoptimization.optimized.ll

bool runOnBasicBlockMultiInstructionOptimization(BasicBlock &B) 
{
    for (auto &Iter : B) // Itera sulle istruzioni del Basic Block
    {
        switch(Iter.getOpcode())
        {
            case Instruction::Add: // Se la prima operazione è una Add
            {
                outs()<<"Ho trovato una Add\n";
                ConstantInt * addConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * addConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (addConstant0 && (!addConstant1)) // Se solo il primo operando della add è una costante
                {
                    outs()<<"Ho trovato una costante in posizione 0 di valore "<<(*addConstant0).getValue()<<"\n"; 
                    
                    // Assegna a NextInstruction il riferimento alla successiva istruzione
                    auto tmpPtr = Iter.getNextNonDebugInstruction();
                    auto &NextInstruction = *tmpPtr;
                    
                    if (NextInstruction.getOpcode() == Instruction::Sub) // Se l'istruzione successiva è una sub
                    {
                        outs()<<"Ho trovato una sub dopo la add\n";
                        ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
                        ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

                        // Se solo il secondo operando della sub è una costante
                        if (nextIntstructionConstant1 && (!nextIntstructionConstant0)) 
                        {
                            outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                            
                            // Se il valore del primo operando della add è uguale al valore del secondo operando della sub
                            if ((*addConstant0).getValue() == (*nextIntstructionConstant1).getValue())
                            {
                                outs()<<"Il primo operando della add è uguale al secondo operando della sub\n";

                                // Sostituisce tutte le occorenze del risultato dell'operazione di sub
                                // con il secondo operatore della add
                                NextInstruction.replaceAllUsesWith(Iter.getOperand(1)); 
                            }
                        }
                    }
               
                }
                if (addConstant1 && (!addConstant0)) // Se solo il secondo operando della add è una costante 
                {
                    outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*addConstant1).getValue()<<"\n";
                    
                    // Assegna a NextInstruction il riferimento alla successiva istruzione
                    auto &NextInstruction = *(Iter.getNextNonDebugInstruction());
                    
                    if (NextInstruction.getOpcode() == Instruction::Sub) // Se l'istruzione successiva è una sub
                    {
                        outs()<<"Ho trovato una sub dopo la add\n";
                        ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
                        ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

                        // Se solo il secondo operando della sub è una costante
                        if (nextIntstructionConstant1 && (!nextIntstructionConstant0)) 
                        {
                            outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                            
                            // Se il valore del secondo operando della add è uguale al valore del secondo operando della sub
                            if ((*addConstant1).getValue() == (*nextIntstructionConstant1).getValue())
                            {
                                outs()<<"Il secondo operando della add è uguale al secondo operando della sub\n";

                                // Sostituisce tutte le occorenze del risultato dell'operazione di sub
                                // con il primo operatore della add
                                NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                            }
                        }
                    }
                }

                break;
            }
            case Instruction::Sub: // Se la prima operazione è una Sub
            {
                outs()<<"Ho trovato una Sub\n";
                ConstantInt * subConstant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * subConstant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                if (subConstant1 && (!subConstant0)) // Se solo il secondo operando è una costante 
                {
                    outs()<<"Ho trovato una costante in posizione 1 di valore "<<(*subConstant1).getValue()<<"\n";

                    // Assegna a NextInstruction il riferimento alla successiva istruzione
                    auto tmpPtr = Iter.getNextNonDebugInstruction();
                    auto &NextInstruction = *tmpPtr;             

                    if (NextInstruction.getOpcode() == Instruction::Add) // Se l'istruzione successiva è una add
                    {
                        outs()<<"Ho trovato una sub dopo la add\n";
                        ConstantInt * nextIntstructionConstant0 = dyn_cast<ConstantInt>(NextInstruction.getOperand(0));
                        ConstantInt * nextIntstructionConstant1 = dyn_cast<ConstantInt>(NextInstruction.getOperand(1));

                        // Se solo il primo operando della add è una costante
                        if (nextIntstructionConstant0 && (!nextIntstructionConstant1)) 
                        {
                            outs()<<"Solo il primo operando della add è una costante che vale "<<(*nextIntstructionConstant0).getValue()<<"\n";
                            
                            // Se il valore del secondo operando della sub è uguale al valore del primo operando della add
                            if ((*subConstant1).getValue() == (*nextIntstructionConstant0).getValue())
                            {
                                outs()<<"Il secondo operando della sub è uguale al primo operando della add\n";

                                // Sostituisce tutte le occorenze del risultato dell'operazione di add
                                // con il primo operatore della sub
                                NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                            }
                        }
                        // Se solo il secondo operando della sub è una costante
                        if (nextIntstructionConstant1 && (!nextIntstructionConstant0)) 
                        {
                            outs()<<"Solo il secondo operando della sub è una costante che vale "<<(*nextIntstructionConstant1).getValue()<<"\n";
                            
                            // Se il valore del secondo operando della sub è uguale al valore del secondo operando della add
                            if ((*subConstant1).getValue() == (*nextIntstructionConstant1).getValue())
                            {
                                outs()<<"Il secondo operando della sub è uguale al secondo operando della add\n";

                                // Sostituisce tutte le occorenze del risultato dell'operazione di add
                                // con il primo operatore della sub
                                NextInstruction.replaceAllUsesWith(Iter.getOperand(0)); 
                            }
                        }
                    }
                }

                break;
            }
            case Instruction::Mul: // Se l'operazione è una Mul
            {
                outs()<<"Ho trovato una Mul\n";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

                break;
            }
            case Instruction::SDiv: // Se l'operazione è una SDiv
            {
                outs()<<"Ho trovato una SDiv\n";
                ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
                ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

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


bool runOnFunctionMultiInstructionOptimization(Function &F) 
{
    bool Transformed = false;

    for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
    {
        if (runOnBasicBlockMultiInstructionOptimization(*Iter)) 
        {
            Transformed = true;
        }
    }

    return Transformed;
}


PreservedAnalyses MultiInstructionOptimizationPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunctionMultiInstructionOptimization(*Iter)) 
        {
            return PreservedAnalyses::none();
        }
    }

  return PreservedAnalyses::none();
}

