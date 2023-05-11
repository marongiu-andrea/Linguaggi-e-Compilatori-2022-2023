#include "LocalOpts.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopPass.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
/*
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
*/


using namespace llvm;

/*Funzione per verificare che due loop abbiano lo stesso numero di iterazioni*/
bool hannoStesseIterazioni(Loop *L1, Loop *L2){
 /* const auto *viaggioL1 = L1->getTripCount();
  const SCEV *viaggioL2 = L2->getTripCount();*/
  //L1->getSmallConstantTripCount();
  //L1->getTripCount();
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
          printf("segs\n");
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

PreservedAnalyses UnioneLoopPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &AM) {

    auto &LI = AM.getResult<LoopAnalysis>(F);
    
    ///per questo esempio specifico ci sono solo 2 loop, metto il cinque come valore più generico
    SmallVector<Loop *, 5> Loops; 
    //LI.getLoopsInPreorder(Loops);
    for(Loop *L : LI){
      Loops.push_back(L);
      //outs()<< *L <<"\n-----\n";
    }
    std::reverse(Loops.begin(),Loops.end());

    for( int i =0; i<Loops.size()-1; i++){
      if(Loops[i]->contains(Loops[i+1])){
        //sono innestati = non adiacenti
      }else{
        outs() << "Loop "<< Loops[i] <<" \n";
        

        //controllo che siano adiacenti
        if(sonoAdiacenti(Loops[i],Loops[i+1])){
          outs()<< "Adiacenti\n";
          if(sonoFlowEquivalent(Loops[i],Loops[i+1])){
            //printf("Sono flow equivalenti e adiacenti\n");
            outs()<< Loops[i] << " e " << Loops[i+1] << " sono adiacenti e flow equivalent\n";
            //Controllo cabbiano lo stesso numero di iterazioni TODO


            
          }else{
            outs()<< Loops[i] << " e " << Loops[i+1] << " non sono adiacenti e flow equivalent\n";
          }
        }
        
      }
    }

    /*
    std::vector<Loop*> listaLoop;
    printf("Inizio Funzione \n");
    
    //controllo di adiacenza
    for( int i = 0; i < listaLoop.size()-1; i++ ){
      Loop* primo = listaLoop[i];
      Loop* secondo = listaLoop[i+1];

      //ill loop deve avere una sola usira
      if(primo->isLoopSimplifyForm() && secondo->isLoopSimplifyForm()){
        printf("E' in forma normalizzata \n");
        BasicBlock *exitPrimo = primo->getExitingBlock();
        BasicBlock *preSecondo = secondo->getLoopPreheader();

        if(preSecondo->getUniquePredecessor() == exitPrimo){
          printf("Sono adiacenti \n");
        }
      }

    }*/

  return PreservedAnalyses::none();
}

// opt -load-pass-plugin=./libLocalOpts.so -passes=unione-loop test/Loop.opt.ll -disable-output 