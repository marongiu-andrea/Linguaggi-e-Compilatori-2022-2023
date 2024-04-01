#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
using namespace llvm;

namespace 
{
  class LoInCoMo final : public LoopPass 
  {
    public:
      static char ID;

      LoInCoMo() : LoopPass(ID) {}

      //funzione virtuale per il calcolo del dominator tree
      virtual void getAnalysisUsage(AnalysisUsage &AU) const override 
      {
        AU.addRequired<DominatorTreeWrapperPass>();
        AU.addRequired<LoopInfoWrapperPass>();
      }

      //funzione che restituisce true se l'istruzione passata è DEAD dopo il loop (ExitBlocks sono i blocchi di uscita fuori dal loop)
      bool checkDeadInst(const Instruction *Iter,SmallVector<BasicBlock *, 1>& ExitBlocks)
      {
        //siccome isUsedInBasicBlock richiede un oggetto Value, facciamo il cast dell'Instruction
        if (auto Val = dyn_cast<Value>(Iter)) 
        {
          //cicliamo i basic block di uscita del loop (ovvero i blocchi subito fuori dal loop)
          for (BasicBlock *ExitBB : ExitBlocks)
          {
            //i basic block di uscita non è detto che siano gli ultimi dell'intero programma, quindi utilizziamo il metodo
            //depth_first che percorre tutti i basic block connessi al blocco di uscita esaminato
            for (auto &BB : depth_first(ExitBB))
            {
              //se la variabile è utilizzata in un blocco significa che non è DEAD
              if ((*Val).isUsedInBasicBlock(BB)) 
              {
                outs()<<"           -      La variabile NON e' dead dopo l'uscita del loop\n";
                return false;
              }
              
              //altrimenti è DEAD
              outs()<<"           -      La variabile e' dead dopo l'uscita del loop\n";
            }
          } 
        }

        //se la variabile non è utilizzata in nessun basic block dopo il loop allora viene restituito true
        return true;
      }

      //funzione per controllare se l'istruzione *Iter è loop invariant 
      //per controllare la catena di istruzioni loop invariant si è utilizzata una lista (loopInvInstructions) per ricordare le istruzioni loop invariant precedenti
      virtual bool checkLoopInvariant(Instruction *Iter, Loop *L, std::list<llvm::Instruction*> &loopInvInstructions)
      {
        //variabile utilizzata per controllare la catena di istruzioni loop invariant
        bool chain = false;
        //siamo sempre sicuri di star controllando istruzioni binarie, ovvero con 2 operandi
        Value *Operand1 = (*Iter).getOperand(0);
        Value *Operand2 = (*Iter).getOperand(1);
        //controlliamo che gli operandi non siano costanti, perchè se lo fossero allora sono loop invariant 
        ConstantInt *C = dyn_cast<ConstantInt>(Operand1);
        ConstantInt *D = dyn_cast<ConstantInt>(Operand2);

        //se gli oggetti sono uguali a nullptr allora sono delle variabili
        outs()<<"   Istruzione : "<<*Iter<<"\n";
        if(C == nullptr)
        {
          //vado a prendere l'istruzione di definizione della variabile esaminata
          Instruction *reference = dyn_cast<Instruction>(Operand1);

          //se la variabile è stata definita allora procedo coi controlli
          if(reference != nullptr) 
          {
            //controllo se la definizione della variabile è stata marcata come loop invariant in precedenza controllando la lista
            for (auto iterLista = loopInvInstructions.begin(); iterLista != loopInvInstructions.end(); iterLista++)
            {
              if(reference->isIdenticalTo(*iterLista))
              {
                chain = true;
                break;
              }
            } 
            
            //se la definizione della variabile è dentro al loop allora l'istruzione che usa la variabile non è loop invariant
            if(!chain && L->contains((*reference).getParent()))
            {
              outs()<<"           -      L'istruzione NON e' loop invariant\n";
              return false;
            }
          }
          
        }

        //controllo allo staesso modo anche l'altra variabile
        if(D == nullptr)
        {
          Instruction *reference = dyn_cast<Instruction>(Operand2);

          if (reference != nullptr)
          {
            for (auto iterLista = loopInvInstructions.begin(); iterLista != loopInvInstructions.end(); iterLista++)
            {
              if(reference->isIdenticalTo(*iterLista))
              {
                chain = true;
                break;
              }
            } 

            if(!chain && L->contains((*reference).getParent()))
            {
              outs()<<"           -      L'istruzione NON e' loop invariant\n";
              return false;
            }
          }
        }

        //se sono arrivato fin qui allora l'istruzione è loop invariant e viene messa nella lista di istruzioni loop invariant
        outs()<<"           -      L'istruzione e' loop invariant\n";
        loopInvInstructions.push_back(&*Iter);

        return true;
      }

      //funzione per controllare che il blocco dell'istruzione domini tutte le uscite del loop
      virtual bool checkDominanceOutput(DominatorTree *DT, BasicBlock *Current, SmallVector<BasicBlock *, 1>& ExitingBlocks)
      {
        //controllo tutti gli exiting blocks (che sono però ancora dentro al loop a differenza degli exit blocks della funzione di prima)
        for (BasicBlock *ExitingBB : ExitingBlocks) 
        {
          //utilizzo il dominator tree per controllare se il blocco dell'istruzione NON domina l'exiting block esaminato, tramite la funzione dominates
          if( !(*DT).dominates(Current, ExitingBB)) {
            outs()<<"           -      L'istruzione NON domina tutte le uscite\n";
            return false;
          }
        }

        //se sono arrivato fin qui allora il basic block dell'istruzione domina tutte le uscite
        outs()<<"           -      L'istruzione domina tutte le uscite\n";
        return true;
      }

      //funzione usata per spostare le istruzioni nel preheader
      bool hoistRegion(Loop *L, std::list<llvm::Instruction*> &HoistList, BasicBlock *preheader) 
      {
        //se la lista di istruzioni da spostare non è vuota
        if (HoistList.empty())
          return false;

        // scorro le istruzioni da spostare
        for (auto I = HoistList.begin(); I != HoistList.end(); I++)
        {
          // se l'istruzione è già nel preheader non la sposto
          if ((*I)->getParent() == preheader)
            continue;

          // muovo l'istruzione dal loop al preheader
          (*I)->moveBefore(preheader->getTerminator());
        }

        //se tutto è andato a buon fine ritorno true
        return true;
      }

      virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override 
      {
        //calcolo il dominator tree e il LoopInfo
        DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree(); // ottengo il DominatorTree
        LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

        //se il loop è in forma 'normale' allora vado avanti
        if((*L).isLoopSimplifyForm())
        {
          //se il preheader non ci fosse non entrerei nell'if
          if(BasicBlock *preheader = (*L).getLoopPreheader())
          {
            //creo un vettore dei basic block di uscita (dentro al loop)
            SmallVector<BasicBlock *, 1> ExitingBlocks;
            L->getExitingBlocks(ExitingBlocks);

            //creo un vettore dei basic block di uscita (fuori dal loop)
            SmallVector<BasicBlock *, 1> SuccBBs;
            L->getExitBlocks(SuccBBs);

            std::list<llvm::Instruction*> loopInvInstructions; //lista di itruzioni loop invariant
            std::list<llvm::Instruction*> movableInst; //lista di istruzioni da spostare nel preheader

            outs()<<"\n";

            //scorro i blocchi del loop
            for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
            {
              BasicBlock *B = *BI;

              //scorro le istruzioni del blocco
              for(auto Iter = (*B).begin(); Iter != (*B).end(); ++Iter )
              {
                //controllo che l'istruzione sia una binary operation
                if( ((*Iter).getOpcode() >= Instruction::BinaryOps::BinaryOpsBegin) && ((*Iter).getOpcode() < Instruction::BinaryOps::BinaryOpsEnd) )
                {
                  // controllo che l'istruzione è loop invariant e, o è dead dopo il loop o domina tutte le uscite 
                  if( checkLoopInvariant(&*Iter, L, loopInvInstructions) && (checkDeadInst(&*Iter,SuccBBs) || checkDominanceOutput(DT,Iter->getParent(),ExitingBlocks)) ) 
                  {
                    outs()<<"           -      L'istruzione può essere spostata nel preheader\n";
                    movableInst.push_back(&*Iter); //se sono entrato in questo if allora metto l'istruzione nella lista di istruzioni da spostare
                  }

                  outs()<<"\n";
                }
                
              }
            }

            //se la funzione ha ritornato false allora significa che non ho dovuto spostare nessuna istruzione
            if(!hoistRegion(L, movableInst, preheader))
              outs()<<"non ci sono istruzioni da spostare\n";
            else
              outs()<<"Il preheader e' composto dalle seguenti istruzioni: \n";

            outs()<<*preheader<<"\n"; //stampo il preheader per mostrare il risultato
          }
        }

        return false; 
      }
  };

  char LoInCoMo::ID = 0;
  RegisterPass<LoInCoMo> X("loop-walk","Nostro Loop Walk");
} // anonymous namespace

