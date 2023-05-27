#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

bool runOnIdentity(BasicBlock &B) 
{
  //creo una lista per cancellare le istruzioni superflue
  std::list<llvm::Instruction*> myInstructions;

  //scorro le istruzioni
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter )
  {
    //variabile utilizzata per le casistiche
    int constantValue = -1;
    
    //qui è stata fatta un'estensione delle operazioni possibili per applicare l'ottimizzazione (non solo add e mul)
    if( ((*Iter).getOpcode() == Instruction::Add) || ((*Iter).getOpcode() == Instruction::Sub) || ((*Iter).getOpcode() == Instruction::Shl) || ((*Iter).getOpcode() == Instruction::LShr) )
    {
      constantValue = 0;
    }
    else if( ((*Iter).getOpcode() == Instruction::Mul) || ((*Iter).getOpcode() == Instruction::SDiv))
    {
      constantValue = 1;
    }

    if(constantValue != -1)
    {
      //estrapolo gli operandi dell'istruzione
      Value *Operand1 = (*Iter).getOperand(0);
      Value *Operand2 = (*Iter).getOperand(1);

      ConstantInt *C = dyn_cast<ConstantInt>(Operand1); // se non è una costante C sarà nullptr
      ConstantInt *D = dyn_cast<ConstantInt>(Operand2);

      // entro nel primo ramo se l'istruzione è una add o una mul perchè sono commutative, e solo se il primo operando è costante e il secondo no (e ovviamente se il valore costante è l'elemento neutro)
      if( ( ((*Iter).getOpcode() == Instruction::Add) || ((*Iter).getOpcode() == Instruction::Mul)) && C != nullptr && D == nullptr && static_cast<int32_t>((*C).getSExtValue()) == constantValue)
      {
        myInstructions.push_back(&*Iter); //metto nella lista l'istruzione da eliminare
        (*Iter).replaceAllUsesWith(Operand2); 
      } // in questo ramo ci possono finire o le add e mul che hanno il primo operando variabile e il secondo costante, o tutte le altre operazioni che seguono la medesima condizione
      else if(D != nullptr && C == nullptr && static_cast<int32_t>((*D).getSExtValue()) == constantValue) 
      {
        myInstructions.push_back(&*Iter); //metto nella lista l'istruzione da eliminare
        (*Iter).replaceAllUsesWith(Operand1);
      }
    }
  }

  //scorro la lista per eliminare le istruzioni superflue
  for (auto iter = myInstructions.begin(); iter != myInstructions.end(); iter++) 
  {
    (*iter)->eraseFromParent();
  }

  return true;
}


bool runOnFunctionIdentity(Function &F) 
{  
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
  {
    if (runOnIdentity(*Iter)) 
    { 
      Transformed = true;
    }
  }

  return Transformed;
}

//nome della classe cambiata che sarà da definire in localOpts.h
PreservedAnalyses IdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) 
  {
    if (runOnFunctionIdentity(*Iter)) 
    { 
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

