#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

// Strength Reduction:
//  15 × 𝑥 = 𝑥 × 15 ⇒ (𝑥 ≪ 4) – 𝑥
//  y = x / 8 ⇒ y = x >> 3

// opt -load-pass-plugin=./libLocalOpts.so -passes=strengthreduction test/test.ll -o test/test.strengthreduction.optimized.bc
// llvm-dis test/test.strengthreduction.optimized.bc -o test/test.strengthreduction.optimized.ll

void mulStrengthReduction(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una Mul:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    if (constant0 && (!constant1)) // Se solo il primo operando è una costante
    {
        outs()<<"Solo il primo operando è una costante di valore "<<(*constant0).getValue()<<"\n";
        if ((*constant0).getValue().exactLogBase2() != -1) // Se il primo operando è una potenza del 2
        {
            outs()<<"Il primo operando è esprimibile come potenza del 2: 2^"<<(*constant0).getValue().logBase2()<<"\n";

            // Crea una nuova istanza di un Value * contenente il valore newVal dello shift
            LLVMContext &ctx = Iter.getOperand(0)->getContext();  
            ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant0).getValue().logBase2()));  
            Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

            // Crea della nuova istruzione
            Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(1), value);

            // Inserisce la nuova istruzione di shift dopo la Mul
            NewLeftShift->insertAfter(&Iter);

            // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro corrispondente allo shift
            Iter.replaceAllUsesWith(NewLeftShift);
        }
        else // Il primo operando non è una potenza del 2
        {   
            // Se il logaritmo è più grande di 1 rispetto alla costante nel primo 
            //  parametro ed è ottenuto con calcolo del logaritmo per eccesso (Ceil) 
            // ---------------------------------------------------------------------------------------
            //   Es: %0 = mul 31, %1
            //    Logaritmo più vicino al primo parametro: 32
            //     In questo caso il logaritmo è più grande di 1, quindi dopo aver generato la nuova 
            //      istruzione di shift sinistro, sarà necessario sottrarre al risultato della shift %1 
            if (((2 << ((*constant0).getValue().ceilLogBase2()-1)) - (*constant0).getValue()) == 1) 
            {
                outs()<<"Il primo operando NON è esprimibile come potenza del 2\n"<<"Potenza del 2 più vicina: 2^"<<(*constant0).getValue().ceilLogBase2()<<"\n";
                // Crea una nuova istanza di un Value * contenente il valore newVal dello shift 
                LLVMContext &ctx = Iter.getOperand(0)->getContext();  
                ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant0).getValue().ceilLogBase2()));  
                Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

                // Crea la nuova istruzione di shift
                Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(1), value);

                // Inserisce la nuova istruzione di shift dopo la Mul
                NewLeftShift->insertAfter(&Iter);

                // Crea la nuova istruzione sub 
                Instruction *NewSub = BinaryOperator::Create(Instruction::Sub, NewLeftShift, Iter.getOperand(1));

                // Inserisce la nuova istruzione sub dopo lo shift
                NewSub->insertAfter(NewLeftShift);

                // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro della sub 
                Iter.replaceAllUsesWith(NewSub);
            }
            else
            {
                outs()<<"Il primo operando NON è esprimibile come potenza del 2\n"<<"Potenza del 2 più vicina: 2^"<<(*constant0).getValue().logBase2()<<"\n";
                // Se il logaritmo è più piccolo di 1 rispetto alla costante nel primo 
                //  parametro ed è ottenuto con calcolo del logaritmo per difetto (Floor)  
                // ---------------------------------------------------------------------------------------
                //   Es: %0 = mul 33, %1
                //    Logaritmo più vicino al primo parametro: 32
                //     In questo caso il logaritmo è più piccolo di 1, quindi dopo aver generato la nuova
                //      istruzione di shift sinistro, sarà necessario aggiungere al risultato della shift %1
                if ((*constant0).getValue() - (2 << ((*constant0).getValue().logBase2()-1)) == 1)
                {
                    // Crea una nuova istanza di un Value * contenente il valore newVal dello shift 
                    LLVMContext &ctx = Iter.getOperand(0)->getContext();  
                    ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant0).getValue().logBase2()));  
                    Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

                    // Crea la nuova istruzione di shift
                    Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(1), value);

                    // Inserisce la nuova istruzione di shift dopo la Mul
                    NewLeftShift->insertAfter(&Iter);

                    // Crea la nuova istruzione di add 
                    Instruction *NewAdd = BinaryOperator::Create(Instruction::Add, NewLeftShift, Iter.getOperand(1));

                    // Inserisce la nuova istruzione di add dopo lo shift
                    NewAdd->insertAfter(NewLeftShift);

                    // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro dell'add 
                    Iter.replaceAllUsesWith(NewAdd);
                } 
            } 
        }            
    }
    if ((!constant0) && constant1) // Se solo il secondo operando è una costante 
    {
        outs()<<"Solo il secondo operando è una costante di valore "<<(*constant1).getValue()<<"\n";
        if ((*constant1).getValue().exactLogBase2() != -1) // Se il secondo operando è una potenza del 2
        {
            outs()<<"Il secondo operando è esprimibile come potenza del 2: 2^"<<(*constant1).getValue().logBase2()<<"\n";

            // Crea una nuova istanza di un Value * contenente il valore newVal dello shift
            LLVMContext &ctx = Iter.getOperand(1)->getContext();  
            ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant1).getValue().logBase2()));  
            Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

            // Crea della nuova istruzione
            Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(0), value);

            // Inserisce la nuova istruzione di shift dopo la Mul
            NewLeftShift->insertAfter(&Iter);

            // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro corrispondente allo shift
            Iter.replaceAllUsesWith(NewLeftShift);
        }
        else // Il secondo operando non è una potenza del 2
        {   

            // Se il logaritmo è più grande di 1 rispetto alla costante nel secondo 
            //  parametro ed è ottenuto con calcolo del logaritmo per eccesso (Ceil) 
            // ---------------------------------------------------------------------------------------
            //   Es: %0 = mul nsw i32 %1, 31
            //    Logaritmo più vicino al secondo parametro: 32
            //     In questo caso il logaritmo è più grande di 1, quindi dopo aver generato la nuova 
            //      istruzione di shift sinistro, sarà necessario sottrarre al risultato della shift %1 
            if (((2 << ((*constant1).getValue().ceilLogBase2()-1)) - (*constant1).getValue()) == 1) 
            {
                outs()<<"Il secondo operando NON è esprimibile come potenza del 2\n"<<"Potenza del 2 più vicina: 2^"<<(*constant1).getValue().ceilLogBase2()<<"\n";
                // Crea una nuova istanza di un Value * contenente il valore newVal dello shift 
                LLVMContext &ctx = Iter.getOperand(1)->getContext();  
                ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant1).getValue().ceilLogBase2()));  
                Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

                // Crea la nuova istruzione di shift
                Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(0), value);

                // Inserisce la nuova istruzione di shift dopo la Mul
                NewLeftShift->insertAfter(&Iter);

                // Crea la nuova istruzione sub 
                Instruction *NewSub = BinaryOperator::Create(Instruction::Sub, NewLeftShift, Iter.getOperand(0));

                // Inserisce la nuova istruzione sub dopo lo shift
                NewSub->insertAfter(NewLeftShift);

                // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro della sub 
                Iter.replaceAllUsesWith(NewSub);
            }
            else
            {
                outs()<<"Il secondo operando NON è esprimibile come potenza del 2\n"<<"Potenza del 2 più vicina: 2^"<<(*constant0).getValue().logBase2()<<"\n";
                // Se il logaritmo è più piccolo di 1 rispetto alla costante nel secondo 
                //  parametro ed è ottenuto con calcolo del logaritmo per difetto (Floor)  
                // ---------------------------------------------------------------------------------------
                //   Es: %0 = mul nsw i32 %1, 33
                //    Logaritmo più vicino al secondo parametro: 32
                //     In questo caso il logaritmo è più piccolo di 1, quindi dopo aver generato la nuova
                //      istruzione di shift sinistro, sarà necessario aggiungere al risultato della shift %1
                if ((*constant1).getValue() - (2 << ((*constant1).getValue().logBase2()-1)) == 1)
                {
                    // Crea una nuova istanza di un Value * contenente il valore newVal dello shift 
                    LLVMContext &ctx = Iter.getOperand(1)->getContext();  
                    ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant1).getValue().logBase2()));  
                    Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

                    // Crea la nuova istruzione di shift
                    Instruction *NewLeftShift = BinaryOperator::Create(Instruction::Shl, Iter.getOperand(0), value);

                    // Inserisce la nuova istruzione di shift dopo la Mul
                    NewLeftShift->insertAfter(&Iter);

                    // Crea la nuova istruzione di add 
                    Instruction *NewAdd = BinaryOperator::Create(Instruction::Add, NewLeftShift, Iter.getOperand(0));

                    // Inserisce la nuova istruzione di add dopo lo shift
                    NewAdd->insertAfter(NewLeftShift);

                    // Rimpiazza tutti gli usi del registro corrispondente alla Mul con il registro dell'add 
                    Iter.replaceAllUsesWith(NewAdd);

                }
            }
        }
    }
}

void sdivStrengthReduction(Instruction &Iter)
{
    outs()<<"----------------------------------------------------------------------------\nHo trovato una SDiv:"<<Iter<<"\n";
    ConstantInt * constant0 = dyn_cast<ConstantInt>(Iter.getOperand(0));
    ConstantInt * constant1 = dyn_cast<ConstantInt>(Iter.getOperand(1));

    // Per evitare che la complessità dell'analisi superasse quella effettiva del calcolo, 
    //  per questo caso abbiamo preso in considerazione solo l'eventualità nella quale 
    //   il secondo operando è esprimibile esattamente come una potenza del 2
    if ((!constant0) && constant1) //Se solo il secondo operando è una costante 
    {
        outs()<<"\nSolo il secondo operando è una costante di valore "<<(*constant1).getValue()<<"\n";
        
        if ((*constant1).getValue().exactLogBase2() != -1) // Se il secondo operando è una potenza del 2
        {
            outs()<<"Il secondo operando è esprimibile come potenza del 2: 2^"<<(*constant1).getValue().logBase2()<<"\n";

            // Crea una nuova istanza di un Value * contenente il valore newVal dello shift
            LLVMContext &ctx = Iter.getOperand(1)->getContext();  
            ConstantInt *newVal = ConstantInt::get(ctx, APInt(32, (*constant1).getValue().logBase2()));  
            Value *value = ConstantExpr::getCast(Instruction::CastOps::SExt, newVal, Type::getInt32Ty(ctx));

            // Crea della nuova istruzione
            Instruction *NewRightShift = BinaryOperator::Create(Instruction::LShr, Iter.getOperand(0), value);

            // Inserisce la nuova istruzione di shift dopo la SDiv
            NewRightShift->insertAfter(&Iter);

            // Rimpiazza tutti gli usi del registro corrispondente alla SDiv con il registro corrispondente allo shift
            Iter.replaceAllUsesWith(NewRightShift);
        }                    
    }
}

bool runOnBasicBlockStrengthReduction(BasicBlock &B) 
{
    for (auto &Iter : B) // Itera sulle istruzioni del Basic Block
    {
        switch(Iter.getOpcode())
        {
            case Instruction::Mul: // Se l'operazione è una Mul
            {
                mulStrengthReduction(Iter);
                break;
            }
            case Instruction::SDiv: // Se l'operazione è una SDiv
            {
                sdivStrengthReduction(Iter);
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

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{

    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunctionStrengthReduction(*Iter)) 
        {
            return PreservedAnalyses::none();
        }
    }

  return PreservedAnalyses::none();
}

