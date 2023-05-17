#include "LocalOpts.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopPass.h"

#include "llvm/Analysis/LoopInfo.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
//per conto loop iteration
#include "llvm/Analysis/ScalarEvolution.h"
/*
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
*/


using namespace llvm;

/*Funzione per verificare che due loop abbiano lo stesso numero di iterazioni
Non c'è un metodo che mi da quell'informazione...
alternativa, controllo le branch instruction dell'header*/
bool hannoStesseIterazioni(llvm::Loop *Loop1, llvm::Loop *Loop2, ScalarEvolution &SE){
  if( SE.getSmallConstantTripCount(Loop1) == SE.getSmallConstantTripCount(Loop2)){
   
    //cntrollo per un bound di cicli
    if( SE.getSmallConstantMaxTripCount(Loop1) == SE.getSmallConstantMaxTripCount(Loop2)){
      return true;
    }
    
  }
  return false;
}

/*controlla che abbiano lo stesso guard branch
Work smart, not harder*/
bool sonoFlowEquivalent(Loop *L1, Loop *L2){
  if (L1->getLoopGuardBranch() != L2->getLoopGuardBranch())
    return false;
  
  return true;
}

bool sonoAdiacenti(Loop *L1, Loop* L2){

  //BasicBlock *EL1 = L1->getExitingBlock();
  BasicBlock *EL1 = L1->getUniqueExitBlock();
  //BasicBlock *EL1 = L1->getLoopPreheader();
  BasicBlock *PHL2 = L2->getLoopPreheader();
  //BasicBlock *PHL1 = L1->getLoopPreheader();
  unsigned int nUscite=0;
  
  //outs() << *EL1<< "\n --------- \n" <<*PHL2 <<"\n";
  //controllo che l'uscita di L1 sia il preheader di L2
  if( EL1 != PHL2)
    return false;

  //COntrollo che dal preheader di L1 si possa solo andare in L2, e da solo un punto
  for(auto Iter = PHL2->begin(); Iter != PHL2->end(); ++Iter){
    Instruction *inst = dyn_cast<Instruction>(Iter);
    
    if(BranchInst* BI = dyn_cast<llvm::BranchInst>(inst)){
      if(BI->isConditional() || BI->isUnconditional()){
        //outs() << *inst << "\n";
        if(BI->getNumSuccessors() > 1)
          return false;
        if(BI->getSuccessor(0) == L2->getHeader()){
          
        }
        nUscite++;
        if(nUscite>1){
          //c'è più di un uscita
          return false;
        }
      }
    }

  }
  return true;
}

/*Placeholder per futuro*/
bool nonHannoDipendenzeNegative(Loop* L1, Loop* L2){
  return true;
}

PreservedAnalyses UnioneLoopPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &AM) {

    auto &LI = AM.getResult<LoopAnalysis>(F);
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    
    
    ///per questo esempio specifico ci sono solo 2 loop, metto il cinque come valore più generico
    SmallVector<Loop *, 5> Loops;
    SmallVector<Loop *, 5> LoopsSorgente;
    SmallVector<Loop *,5> LoopsDestinazione;
    //LI.getLoopsInPreorder(Loops);
    for(Loop *L : LI){
      Loops.push_back(L);
      //outs()<< *L <<"\n-----\n";
    }
    //metto i loop in preorder nel vettore
    std::reverse(Loops.begin(),Loops.end());

    for( int i =0; i<Loops.size()-1; i++){
      if(Loops[i]->contains(Loops[i+1])){
        //sono innestati = non adiacenti
      }else{
        //outs() << "Loop "<< Loops[i] <<" \n";
        
        //controllo che siano adiacenti
        if(sonoAdiacenti(Loops[i],Loops[i+1])){
          outs()<< "Adiacenti\n";
          if(sonoFlowEquivalent(Loops[i],Loops[i+1])){
            //printf("Sono flow equivalenti e adiacenti\n");
            outs()<< Loops[i] << " e " << Loops[i+1] << " sono adiacenti e flow equivalent\n";
            //Controllo cabbiano lo stesso numero di iterazioni TODO
            if(hannoStesseIterazioni(Loops[i],Loops[i+1],SE)){
              if(nonHannoDipendenzeNegative(Loops[i],Loops[i+1])){
                outs()<< "Hanno le stesse iterazione E nessuna dipendenza negativa, SI POSSONO UNIRE\n";
                LoopsSorgente.push_back(Loops[i]);
                LoopsDestinazione.push_back(Loops[i+1]);
              }

            }
          }else{
            outs()<< Loops[i] << " e " << Loops[i+1] << " non sono adiacenti e flow equivalent\n";
          }
        }
        
      }
    }

    //li voglio in ordine giusto
    std::reverse(LoopsSorgente.begin(),LoopsSorgente.end());
    std::reverse(LoopsDestinazione.begin(),LoopsDestinazione.end());

    for(int i = 0; i<LoopsSorgente.size(); i++){
      //se il loop è sparito durante un unione salto il passaggio, questo
      //per evitare errori
      if(LoopsSorgente[i] == nullptr || LoopsDestinazione[i] == nullptr)
        continue;

      Loop *Sorgente = LoopsSorgente[i];
      Loop* Destinazione = LoopsDestinazione[i];
      
      /*Più avanti dovrò sostituire in sorgente di destinazione col nuovo loop creato perche
      se A si può unire a B e B si può unire ad C allora anche A si può unire a C, e di conseguenza
      anche AB*/
      
      for(int i = 0; i<LoopsDestinazione.size(); i++){
        if(LoopsDestinazione[i] == Destinazione){
          //LoopsDestinazione[i] = nullptr;
          //TODO marco per sostituzione dopo
        }
      }
      
      //uniamo i loop
      //Sorgente->getInductionVariable(SE);
      
      PHINode *IVDest= Destinazione->getInductionVariable(SE);
      
      
     // outs() << "controllo: " << IVDest<< "\n";
      //IVDest->replaceAllUsesWith(Sorgente->getInductionVariable(SE));

      
      for(auto *BB : Destinazione->getBlocks()){
        
        outs() << *BB << "\n ------ \n";
      }
      

    } 
    
  return PreservedAnalyses::none();
}

// opt -load-pass-plugin=./libLocalOpts.so -passes=unione-loop test/Loop.opt.ll -disable-output 