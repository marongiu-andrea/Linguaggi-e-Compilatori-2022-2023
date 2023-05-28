
#include "llvm/IR/Dominators.h"
#include <llvm/Analysis/LoopPass.h>
#include "llvm/IR/InstrTypes.h"
#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/PostDominators.h"
#include <vector>
#include <queue>
using namespace llvm;

#define vv

/**
 * @brief Ritorna vettore uscite e stampa
 * @param L 
 * @return 
 */
static void printLoopINBlocksgetExit(Loop *L) {
  SmallVector<BasicBlock*> allDom;
  for (Loop::block_iterator BI = L->block_begin() ; BI != L->block_end();  ++BI) {
    allDom.push_back(*BI); 
    BasicBlock &p = (**BI);  L->getExitBlocks( allDom );
    #ifdef vvv
    outs() << p ; outs() << "\n";
    #endif
  }
}



/**
 * @brief Modifica il CFG linkando i blocchi sui rami adatti
 * @param B1 precedente
 * @param B2 sucessore
 * @param ramo  vero falso
 */
void modLinkBlocks (BasicBlock* B1 , BasicBlock* B2 , const bool ramo ) {  
    (dyn_cast<BranchInst>(B1->getTerminator()))->setSuccessor(ramo,B2) ;
  }



/**
 * @brief linka la nuova exit comune tra i 2 loop al header unico dei 2 loop
 * @param UH 
 * @param PCEL 
 */
void attachNextTrueBranch(BasicBlock* B1 , BasicBlock* B2) { //outs() <<   
  modLinkBlocks(B1,B2,true) ; 
  }

/**
 * @brief linka la nuova exit comune tra i 2 loop al header unico dei 2 loop
 * @param LB 
 * @param PL 
 */
void attachNextFalseBranch(BasicBlock* B1 , BasicBlock* B2)  { modLinkBlocks(B1,B2,false) ;  }


/**
 * @brief linka la nuova exit comune tra i 2 loop al header unico dei 2 loop
 * @param LB 
 * @param PL 
 */
void successorToNull(BasicBlock* B , const bool ramo )  { 
  (dyn_cast<BranchInst>(B->getTerminator()))->setSuccessor(ramo,nullptr) ;
}


/**
 * @brief Integrata in LLVM non c'era. Questa non funziona sempre, si rompe in casi più complessi.
 * @param LL 
 * @return 
 */
BasicBlock* getBody( Loop * LL ) {return (dyn_cast<BranchInst>(LL->getHeader()->getTerminator()))->getSuccessor(false); }

/**
 * @brief scambia gli usi della induction variable, dovrebbe funzionare sempre
 * @param LL 
 * @return 
 */
void replaceUsesIndictionVariable(Loop * L1, Loop * L2 ) {
    dyn_cast<PHINode>(L2->getHeader()->begin())->replaceAllUsesWith(dyn_cast<PHINode>(L1->getHeader()->begin()));
    dyn_cast<PHINode>(L2->getHeader()->begin())->eraseFromParent(); 
  }





/**
 * @brief Funzione per fondere i loop 
 * 
 * ALGORITMO
 * fuseLoops(Function F) 
 * entry
 * – for each nest level NL, outermost to innermost
 *    Collect loops that are candidates for loop fusion at NL
 *    Sort candidates into control-flow equivalent sets
 * 
 * Come li fondo?
 * 
 * Modifico le induction variablile
 * scrivo nel body di uno il body dell'altro poi quello non modificato 
 * lo lascio lì tanto non viene eseguito
 * 
 * La dominanza serve per vedere quale loop viene prima, 
 * qua sono entrambi dominant e post dominant
 * 
 * verrà tenuto il loop 1
 * 
 * @param F 
 * @return 
 */
void FuseLoops(Loop* l1, Loop* l2  )  {

  // rimpiazza usi induction variabile
  replaceUsesIndictionVariable(l1,l2);

  // sceglie blocchi definitivi
  BasicBlock * exit = l2->getExitBlock();
  BasicBlock *  latch = l1->getLoopLatch();
  BasicBlock *  header = l1->getHeader();

  //outs() << * exit << "get exit\n"; 


  // attacca i body 
  BasicBlock *  b1 = getBody(l1);
  BasicBlock *  b2 = getBody(l2);
  attachNextFalseBranch(b1,b2);

  //stacca inutili
  //BasicBlock *  in1 = l2->getLoopLatch();
  //BasicBlock *  in2 = l2->getLoopPreheader();
  //BasicBlock *  in3 = l2->getHeader();
  //successorToNull(in1, false);
  //successorToNull(in2 , false);
  //successorToNull(in3 , false);

  // attacca il body 2 al latch definitivo
  attachNextFalseBranch(b2, latch ) ;

  // attacca uscita
  attachNextTrueBranch( header, exit ) ;

}



/**
 * @brief Scrivere condizioni di adiacenza
 * 
 * come controllare se loop sono adiacenti?
 * condizione di adiacenza se le istruzini
 * vanno nel preheader
 * deve contenere solo una branch e la branch
 * deve saltare dentro il loop successivo
 * (adiacente) con al massimo un subset di 
 * istruzioni lineari senza altre uscite in mezzo
 * 
 * - nel preheader deve contenere solo una branch
 * - la branch deve saltare dentro il loop successivo
 * OR
 * - possono esistere istruzioni lineari senza altre uscite in mezzo
 * 
 * lo spiegozzo è alla slide del Barton-LoopFusion pagina 6
 * 
 * @param l1 
 * @param l2 
 * @return 
 */
static bool areAdiacent(Loop* l1, Loop* l2 ) {
  
  /**Hanno un unico blocco di uscita*/
  if (!l1->getUniqueExitBlock()) return false;// prevent dump 
  if (!l2->getUniqueExitBlock()) return false;// prevent dump 
  
  /**Prende i blocchi e ne trova il bloccco di uscita se unico*/

  if (l1->getLoopPreheader() == l2->getExitBlock() ) return true;
  if (l2->getLoopPreheader() == l1->getExitBlock() ) return true;

  /**manca il controllo se esiste uno o più blocchi in mezzo senza altre uscite*/

  return false;
}


/**
 * @brief il loop è rotto?
 * @param L 
 * @return 
 */
static bool isValidLoop(Loop * L) {
  if (L->isInvalid()) false; // prevent dump
  if (!L->getLoopPreheader()) false; // se non ha il preheader, salta
  if (!L->getHeader()) false; // se non ha l'header, salta
  if (!L->getLoopLatch()) false;  // se non ha il latch, salta
  if ( !(L->getLoopLatch())->getUniquePredecessor()  ) false;  // se non ha il body, salta
  return true;
}

  /**
   * @brief Vero se hanno lo sesso Trip Count
   * @param l1 
   * @param l2 
   * @param SE 
   * @return 
   */
  static bool hasInductionVariabileTheSameSize( Loop* l1, Loop* l2  ,  ScalarEvolution &SE) {
    return (SE.getSmallConstantTripCount(l1) == SE.getSmallConstantTripCount(l2) );
  } 

/**
 * @brief Se il primo loop domina il secondo
 * @param l1 
 * @param l2 
 * @param DT 
 * @return 
 */
static bool areDomintant( Loop* l1, Loop* l2  ,  DominatorTree  &DT ) { return DT.dominates(l1->getHeader(),l2->getHeader() ) ;}

/**
 * @brief Se il primo loop è postdominato dal secondo
 * @param l1 
 * @param l2 
 * @param PDT 
 * @return 
 */
static bool arePostDomintant( Loop* l1, Loop* l2,  PostDominatorTree &PDT ) { return PDT.dominates(l1->getHeader(),l2->getHeader() ); }



  /**
   * @brief Vero se i 2 loop sono dominant e postdominant cioè control flow equivalenti
   * 
   * 
   * @param l1 
   * @param l2 
   * @param DT 
   * @param PDT 
   * 
   * @return 
   */
  static bool areDomintantAndPostDominant( Loop* l1, Loop* l2  ,
   DominatorTree  &DT , PostDominatorTree &PDT) { 
    return (areDomintant(l1,l2,DT) &&  arePostDomintant(l2, l1 , PDT)) || 
    (areDomintant(l2,l1,DT) &&  arePostDomintant(l1, l2 , PDT)) ; 
    } 

/**
 * @brief Quale dei due loop precede l'altro?
 * @param l1 
 * @param l2 
 * @return 
 */
Loop **  isSymmetricTest(Loop* l1, Loop* l2,DominatorTree  &DT) {
  const unsigned N = 2;
  Loop** R = new Loop*[N];
  R[0],R[1] = nullptr;
  if (areDomintant(l1,l2,DT)) { R[0]=l1; R[1]=l2 ;}
  if (areDomintant(l2,l1,DT)) { R[0]=l2; R[1]=l1 ;}
  return R;
}


  /**
   * @brief  ritorna loop adiacenti
   * 
   * esegue il passo
   * 
    */
  void executePass ( SmallVector<llvm::Loop *> Lv , DominatorTree  &DT , PostDominatorTree &PDT , ScalarEvolution &SE ) {
    SmallVector<llvm::Loop *> R;
    static llvm::Loop * ap=nullptr;
    for (auto iter : Lv  ) { 
      
      
      if ( ap == nullptr)   { ap=iter; continue;}// inserisce il primo
      if (!isValidLoop(iter)) continue; //salta al prossimo loop, nel nostro caso non dovrebbe servire, 
                                        // ma in generale è meglio controllare che il loop sia in stato corretto


      if (areAdiacent (ap , iter ) &&  
          hasInductionVariabileTheSameSize (ap ,iter, SE ) && 
          areDomintantAndPostDominant( ap, iter , DT , PDT) 
          ) {
        #ifdef vvv
        outs() << "------------------Stampa Loop Fusi sotto\n\n" ;
        #endif
        Loop ** L =  isSymmetricTest(ap,iter,DT);
        if (L[1]) { FuseLoops(L[0],L[1]);
          printLoopINBlocksgetExit(L[0]);
          printLoopINBlocksgetExit(L[1]);
        }
        else outs() << "2 loop non fondibili ora" ;
        delete [] L;
      }

    }

  }










/**
 * @brief Converte il loop di vettori Se non c'è il cast tra vector e SmallVectr allora lo creiamo noi
 * 
 * Stampa
 * 
 * scritta male, verra ottimizzata dal compilatore (=
 * 
 * @param Lv 
 * @return 
 */
SmallVector<llvm::Loop *> moveIntoSmallVectorPrint ( SmallVector<llvm::Loop *, 4U> Lv) {
  SmallVector<llvm::Loop *> r;
  for (auto iterLoop : Lv )  r.push_back (iterLoop) ;
  for (auto iterLoop : r )  if (!iterLoop->isLoopSimplifyForm()) r.pop_back(); 
  #ifdef vvv
  outs()<<"Intestazioni (usare per muovere loop)\n";
  for (auto iterLoop : Lv )  outs () << (*iterLoop) ;
  outs()<<"\n\n";
  for (auto iterLoop : Lv )  printLoopINBlocksgetExit(iterLoop) ;
  outs()<<"\n\n";
  #endif
  return r;
}



PreservedAnalyses TransformPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &AM) {
    auto &LI = AM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
  PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
  ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

    /**Crea un vettore di puntatori a loops, quasi inutile nel nostro caso, ne abbiamo solo 2*/
    auto aal = LI.getLoopsInPreorder();
    SmallVector<llvm::Loop *> manualCast = moveIntoSmallVectorPrint(aal) ;

    /*Esegue passo*/
    executePass ( manualCast, DT , PDT ,SE );


    return PreservedAnalyses::all();
}
