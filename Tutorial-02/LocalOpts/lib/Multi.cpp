#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

bool runOnMulti(BasicBlock &B) 
{ 
  //lista per l'eliminazione delle istruzioni superflue
  std::list<llvm::Instruction*> myInstructions;

  //for per il ciclo delle istruzioni
  for(auto Iter = B.begin(); Iter != B.end(); ++Iter )
  {
    // Controllo se l'istruzione è una sub o una signed div, il trattamento è simile
    if((*Iter).getOpcode() == Instruction::Sub || (*Iter).getOpcode() == Instruction::SDiv)
    {
      //salvo gli operandi del right-hand side dell'istruzione
      Value *Operand1 = (*Iter).getOperand(0);
      Value *Operand2 = (*Iter).getOperand(1);

      //per applicare i cambiamenti il secondo operando dev'essere costante, 
      //l'assegnamento seguente restituisce nullptr se l'operando non è una costante
      ConstantInt *C = dyn_cast<ConstantInt>(Operand2);
      if( ( C != nullptr ) && !( dyn_cast<ConstantInt>(Operand1) ) ) //in questo caso il primo operando (Operand1) NON dev'essere una costante mentre il secondo si (Operand2 chiamato anche *C)
      {
        //facendo questo cast ottengo l'istruzione di assegnamento dell'operando in questione, scritta precedentemente
        //se l'operando è la variabile 'b' , otterrò l'istruzione di assegnamente scritta in precedenza di 'b'
        Instruction *prevInst = dyn_cast<Instruction>(Operand1);

        // se l'istruzione di assegnamento è una Mul o una Add allora continuo a controllare
        if( (*prevInst).getOpcode() == Instruction::Mul || (*prevInst).getOpcode() == Instruction::Add )
        {
          //estrapolo gli operandi del right-hand side dell'istruzione in questione
          Value *prevOperand1 = (*prevInst).getOperand(0);
          Value *prevOperand2 = (*prevInst).getOperand(1);

          ConstantInt *Y = dyn_cast<ConstantInt>(prevOperand1);
          ConstantInt *Z = dyn_cast<ConstantInt>(prevOperand2);

          //controllo che il primo operando (Y) NON sia costante e il secondo SI (Z)
          //ed in più controllo che la costante SOMMATA nell'istruzione precedente (*Z) sia uguale alla costante SOTTRATTA nell'istruzione attuale (*C)
          if( (Y == nullptr) && ( (Z != nullptr) && (*Z).getSExtValue() == (*C).getSExtValue() ) )
          {
            myInstructions.push_back(&*Iter); //pusho l'istruzione da cancellare nella lista
            (*Iter).replaceAllUsesWith((*prevInst).getOperand(0));
          }
          else if( ( (Y != nullptr) && (*Y).getSExtValue() == (*C).getSExtValue() ) && (Z == nullptr) ) // guardo quali dei 2 op è uguale e mi assicuro che l'altra sia una var
          {
            //metto l'istruzione da cancellare nella lista e poi ricreo l'istruzione in modo corretto facendo replaceAllUsesWith
            myInstructions.push_back(&*Iter);
            (*Iter).replaceAllUsesWith((*prevInst).getOperand(1)); 
          }
        }
      }
    } // Controllo se l'istruzione attuale è una add o di una mul, il trattamento è simile
    else if((*Iter).getOpcode() == Instruction::Add || (*Iter).getOpcode() == Instruction::Mul)
    {
      //effettuo lo stesso controllo sugli operandi come in precedenza
      Value *Operand1 = (*Iter).getOperand(0);
      Value *Operand2 = (*Iter).getOperand(1); 

      ConstantInt *C = dyn_cast<ConstantInt>(Operand1);
      ConstantInt *D = dyn_cast<ConstantInt>(Operand2);

      //controllo la prima casistica (primo operando costante e il secondo no)
      if( ( C != nullptr ) && ( D == nullptr ) )
      { 
        // guardo il riferimento del primo op (l'istruzione dio assegnamento precedente)
        Instruction *prevInst = dyn_cast<Instruction>(Operand2); 
        
        // se il riferimento è una sub o una signed div allora continuo a controllare
        if( (prevInst) && ((*prevInst).getOpcode() == Instruction::Sub || (*prevInst).getOpcode() == Instruction::SDiv) ) 
        {
          //analizzo l'istruzione di assegnamento precedente
          Value *prevOperand1 = (*prevInst).getOperand(0);
          Value *prevOperand2 = (*prevInst).getOperand(1);

          ConstantInt *Y = dyn_cast<ConstantInt>(prevOperand1);
          ConstantInt *Z = dyn_cast<ConstantInt>(prevOperand2);

          //controllo se gli operandi sono costanti o meno e controllo l'uguaglianza delle costanti dell'istruzione precedente e quella attuale
          if( (Y == nullptr) && ( (Z != nullptr) && (*Z).getSExtValue() == (*C).getSExtValue() ) ) 
          {
            //mi salvo l'istruzione da cancellare e faccio una raplace
            myInstructions.push_back(&*Iter);
            (*Iter).replaceAllUsesWith((*prevInst).getOperand(0));
          }
        }
      } //controllo l'altra casistica (secondo operando costante e il primo no)
      else if( ( C == nullptr ) && ( D != nullptr ) )
      { 
        // guardo il riferimento del primo op (istruzione di assegnamento precedente)
        Instruction *prevInst = dyn_cast<Instruction>(Operand1); 
        
        // se il riferimento è una Sub o una signed div allora continuo a controllare
        if( (prevInst) && ((*prevInst).getOpcode() == Instruction::Sub || (*prevInst).getOpcode() == Instruction::SDiv) ) 
        {
          //stessi controlli di prima
          Value *prevOperand1 = (*prevInst).getOperand(0);
          Value *prevOperand2 = (*prevInst).getOperand(1);

          ConstantInt *Y = dyn_cast<ConstantInt>(prevOperand1);
          ConstantInt *Z = dyn_cast<ConstantInt>(prevOperand2);

          //controllo sempre di poter applicare la Multi-Instruction Optimization se le costanti nelle 2 istruzioni sono uguali
          if( (Y == nullptr) && ( (Z != nullptr) && (*Z).getSExtValue() == (*D).getSExtValue() ) )
          {
            myInstructions.push_back(&*Iter);
            (*Iter).replaceAllUsesWith((*prevInst).getOperand(0));
          }
        }
      }
    }
  }

  //ciclo la lista creata inizialmente con le istruzioni da eliminare
  for (auto iter = myInstructions.begin(); iter != myInstructions.end(); iter++) 
  {
    //cancello l'istruzione
    (*iter)->eraseFromParent();
  }

  return true;
}


bool runOnFunctionMulti(Function &F) 
{  
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) 
  {
    if (runOnMulti(*Iter)) 
    { 
      Transformed = true;
    }
  }

  return Transformed;
}

//nome della classe cambiata che sarà da definire in localOpts.h
PreservedAnalyses MultiPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) 
{
  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) 
  {
    if (runOnFunctionMulti(*Iter)) 
    { 
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}