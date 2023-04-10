#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;


bool runOnBasicBlock(BasicBlock &B) 
{
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter )
  {
    //Iter è la istruzione che stiamo analizzando
    if( (*Iter).getOpcode() == Instruction::Mul ) // controllo se l'operazione che viene effettuata è una MUL
    {
      //estrapolo il secondo operando dell'istruzione
      Value *Operand = (*Iter).getOperand(1);

      //controllo se è una costante
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) 
      {
        if( static_cast<int32_t>((*C).getSExtValue())%2 == 0)  // ci interessa solo se la moltiplicazione è per multiplo di 2
        {
          //creo la nuova istruzione di shift calcolando il logaritmo
          auto opType = (*C).getType();
          auto newOperand = ConstantInt::get(opType,(*C).getValue().exactLogBase2());
          Instruction *NewInst2 = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(0), newOperand);

          NewInst2->insertAfter(&*Iter);

          (*Iter).replaceAllUsesWith(NewInst2);
          // Si possono aggiornare le singole references separatamente?
          // Controlla la documentazione e prova a rispondere. Decomentare per provare
          // auto lastInst = B.end();
          // --lastInst; // return sono 2 istruzioni
          // --lastInst;
          // (*lastInst).replaceUsesOfWith((&*Iter), NewInst2);
        }
      }
    }
  }

  return true;
}

bool runOnFunction(Function &F) 
{
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
  {
    if (runOnBasicBlock(*Iter)) 
    {
      Transformed = true;
    }
  }

  return Transformed;
}

PreservedAnalyses TransformPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) 
  {
    if (runOnFunction(*Iter)) 
    {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

