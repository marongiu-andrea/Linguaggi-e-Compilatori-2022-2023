#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/ADT/APInt.h"
#include <cmath>

using namespace llvm;

bool runOnStrength(BasicBlock &B) 
{
  //creo la lista per la cancellazione delle istruzioni superflue
  std::list<llvm::Instruction*> myInstructions;

  //ciclo le istruzioni
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter )
  {
    if( (*Iter).getOpcode() == Instruction::Mul ) // controllo se l'operazione che viene effettuata è una MUL
    {
      //estrapolo gli operandi della mul
      Value *Operand1 = (*Iter).getOperand(0);
      Value *Operand2 = (*Iter).getOperand(1);

      // Se ho 2 variabili o 2 costanti numeriche ignoro il passaggio DOV'è FINITO LO XOR?
      if( ConstantInt *C = dyn_cast<ConstantInt>(Operand1) ) // controllo se il primo operando è una costante
      {
        //controllo la casistica della costante che è una potenza di 2
        if( isPowerOf2_64((*C).getZExtValue()) )
        {
          //metto nella lista l'istruzione da cancellare
          myInstructions.push_back(&*Iter);

          //prendo il valore per fare lo shift
          auto shiftBy = log2((*C).getZExtValue());
          Constant *shiftValue = ConstantInt::getSigned(Operand1->getType(),shiftBy);

          //creo la nuova istruzione di shift (dato che la costante è già potenza di 2 non serve fare la sottrazione o la somma)
          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(1), shiftValue);
          NewInst->insertAfter(&*Iter);

          (*Iter).replaceAllUsesWith(NewInst);
        } 
        else if( isPowerOf2_64((*C).getZExtValue() + 1) ) //controllo la casistica per esempio 'variabile x 15' (costante non potenza di 2)
        {
          //metto nella lista l'istruzione da eliminare
          myInstructions.push_back(&*Iter);

          //prendo il valore per fare lo shift
          auto shiftBy = log2((*C).getZExtValue() + 1);
          Constant *shiftValue = ConstantInt::getSigned(Operand1->getType(),shiftBy);

          //creo l'istruzione di shift con la variabile precedente e il numero appena creato
          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(1), shiftValue);
          NewInst->insertAfter(&*Iter);

          //creo anche l'istruzione di sottrazione utilizzando l'istruzione precedente come operando, Esempio: '(var << shiftBy) - var'
          Instruction *NewInst2 = BinaryOperator::Create(Instruction::Sub, NewInst, (*Iter).getOperand(1));
          NewInst2->insertAfter(NewInst);

          (*Iter).replaceAllUsesWith(NewInst2);
        }
        else if( isPowerOf2_64((*C).getZExtValue() - 1) )
        {
          myInstructions.push_back(&*Iter);

          auto shiftBy = log2((*C).getZExtValue() - 1);

          Constant *shiftValue = ConstantInt::getSigned(Operand1->getType(),shiftBy);

          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(1), shiftValue);

		  //qui anzichè creare un'istruzione di sottrazione bisogna creare una somma
          NewInst->insertAfter(&*Iter);
          Instruction *NewInst2 = BinaryOperator::Create(Instruction::Add, NewInst, (*Iter).getOperand(1));
          NewInst2->insertAfter(NewInst);

          (*Iter).replaceAllUsesWith(NewInst2);
        }
      }
	  else if( ConstantInt *C = dyn_cast<ConstantInt>(Operand2) ) // controllo se il secondo operando è una costante
      {
        if( isPowerOf2_64((*C).getZExtValue()) )
        {
          myInstructions.push_back(&*Iter);

          auto shiftBy = log2((*C).getZExtValue());

          Constant *shiftValue = ConstantInt::getSigned(Operand2->getType(),shiftBy);

          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(0), shiftValue);

          NewInst->insertAfter(&*Iter);

          (*Iter).replaceAllUsesWith(NewInst);
        }
        else if( isPowerOf2_64((*C).getZExtValue() + 1) )
        {
          myInstructions.push_back(&*Iter);

          auto shiftBy = log2((*C).getZExtValue() + 1);

          Constant *shiftValue = ConstantInt::getSigned(Operand2->getType(),shiftBy);

          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(0), shiftValue);

          NewInst->insertAfter(&*Iter);

          Instruction *NewInst2 = BinaryOperator::Create(Instruction::Sub, NewInst, (*Iter).getOperand(0));

          NewInst2->insertAfter(NewInst);

          (*Iter).replaceAllUsesWith(NewInst2);
        }
        else if( isPowerOf2_64((*C).getZExtValue() - 1) )
        {
          myInstructions.push_back(&*Iter);

          auto shiftBy = log2((*C).getZExtValue() - 1);

          Constant *shiftValue = ConstantInt::getSigned(Operand2->getType(),shiftBy);

          Instruction *NewInst = BinaryOperator::Create(Instruction::Shl, (*Iter).getOperand(0), shiftValue);

          NewInst->insertAfter(&*Iter);

          Instruction *NewInst2 = BinaryOperator::Create(Instruction::Add, NewInst, (*Iter).getOperand(0));

          NewInst2->insertAfter(NewInst);

          (*Iter).replaceAllUsesWith(NewInst2);
        }
      }
    } //controllo se l'istruzione è una divisione
    else if( (*Iter).getOpcode() == Instruction::SDiv )
    {
      //controllo solo il secondo operando cioè getOperand(1) ( perchè il primo è getOperand(0) ) 
      //perchè posso applicare l'ottimizzazione solo per il denominatore e non per il numeratore
      Value *Operand = (*Iter).getOperand(1);
      if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)) 
      {
        if( isPowerOf2_32(static_cast<int32_t>((*C).getSExtValue())) )  // ci interessa solo se la divisione è per un multiplo di 2
        {
          //non controllo il caso della divisione per 1, che viene già controllato nel passo di identity ( notare il NOT davanti a (*C) )
          if( !(*C).getValue().isOne() )
          {
            //metto nella lista l'istruzione da eliminare
            myInstructions.push_back(&*Iter);
            
            //creo la nuova istruzione di shift, calcolando il logaritmo della costante
            auto opType = (*C).getType();
            auto newOperand = ConstantInt::get(opType,(*C).getValue().exactLogBase2());
            Instruction *NewInst2 = BinaryOperator::Create(Instruction::LShr, (*Iter).getOperand(0), newOperand);
            NewInst2->insertAfter(&*Iter);

            (*Iter).replaceAllUsesWith(NewInst2);
          }
        }
      }
    }
  }

  //scorro la lista creata in precedenza per cancellare le istruzioni superflue
  for (auto iter = myInstructions.begin(); iter != myInstructions.end(); iter++) 
  {
    (*iter)->eraseFromParent();
  }

  return true;
}

bool runOnFunctionStrength(Function &F) 
{  
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
  {
    if (runOnStrength(*Iter)) 
    {
      Transformed = true;
    }
  }

  return Transformed;
}

//nome della classe cambiata che sarà da definire in localOpts.h
PreservedAnalyses StrengthPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) 
  {
    if (runOnFunctionStrength(*Iter)) 
    { 
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

