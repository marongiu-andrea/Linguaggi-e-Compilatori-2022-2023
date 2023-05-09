#include "LocalOpts.h"
#include "llvm/IR/Function.h"
#include "llvm/Analysis/LoopPass.h"

#include "llvm/Analysis/LoopInfo.h"
/*
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
*/


using namespace llvm;

/*Funzione per verificare che due loop abbiano lo stesso numero di iterazioni*/
bool hannoStesseIterazioni(Loop *L1, Loop *L2, ScalarEvolution *SE){
 /* const auto *viaggioL1 = L1->getTripCount();
  const SCEV *viaggioL2 = L2->getTripCount();*/
  return false;
}

/*controlla che abbiano lo stesso guard branch
Work smart, not harder*/
bool sonoFlowEquivalent(Loop *L1, Loop *L2){
  if (L1->getLoopGuardBranch() != L2->getLoopGuardBranch())
    return false;
  
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

    }
    
    for( int i =0; i<Loops.size()-1; i++){
      if(Loops[i]->contains(Loops[i+1])){
        //sono innestati = non adiacenti
      }else{//sono adiacenti
        outs() << "Loop "<< Loops[i] <<" \n";
        
        //Controllo cabbiano lo stesso numero di iterazioni TODO
        //outs() << LI.getTripCount(Loops[i]) <<" \n";

        //controllo che siano FlowEquivalent
        if(sonoFlowEquivalent(Loops[i],Loops[i+1])){
          printf("Sono flow equivalenti \n");
        }else{
          printf("Non sono equivalenti \n");
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