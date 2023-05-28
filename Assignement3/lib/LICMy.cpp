#include "llvm/ADT/SmallVector.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallString.h"

#include "llvm/IR/ValueMap.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ValueTracking.h"
//#includ" <llvm/PassAnalysisSupport.h>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Dominators.h"




#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Value.h>
#include <llvm/Analysis/ValueTracking.h>



#include <vector>
#include <map>

#include "llvm/IR/InstrTypes.h"

#include "llvm/IR/PatternMatch.h" /**< Aggiunto*/

using namespace llvm;

#define v



namespace {


  class LoopWalkPass final : public LoopPass {
    public:
    static char ID;

    LoopWalkPass() : LoopPass(ID) {}


      /**
       * @brief sposta il vettore delle istruzini nel preheader
       * @param lii 
       * @param DT 
       */
      static void checkCodeMotion( BasicBlock *PHB , SmallVector<Instruction*> lii  ) {
        for (auto p :lii) {//outs()<<*p<<"\n";
          auto &i = *PHB->getFirstInsertionPt();
          p->moveBefore(&i) ;
        }
      }

      /**
       * @brief Sono store binarie,con due costanti. 
       * @param I 
       * @return bool
       */
      static bool isBinaryConstantInstStore ( Instruction* I ) {
        // espressione  binaria, due costanti
        return I->isBinaryOp()  &&  static_cast<StoreInst*>( &*I ) &&
            !static_cast<LoadInst*>( &*I )
            &&  dyn_cast<ConstantInt>(I->getOperand(0))  && 
            dyn_cast<ConstantInt>(I->getOperand(1))  ;
      }

      /**
       * @brief vero il loop non coniene tutti gli User di una istruzione
       * @param L 
       * @param I 
       * @return 
       */
      static bool hasUsesDefinedOutsideLoop (Loop *L , Instruction* I) {
        for (User::op_iterator i = I->op_begin(); i != I->op_end();  ++i  ) {
          Instruction * app=dyn_cast<Instruction>( i );
          if (app) // dump if null protection
          if (L->contains( app ))  return false;
        }
        return true;
      }

      /**
       * @brief I domina tutti i blocchi di uscita de DT 
       * 
       * L'intenzione è chiara, spero acnhe la funzionalità
       * 
       * @param DT 
       * @return 
       */
      static bool isExitDominator (Instruction* I ,DominatorTree *DT ,SmallVector<BasicBlock*> eXit) {
        for (auto iterbbEx: eXit ) 
          if ( !DT->dominates(  I ,  iterbbEx )) return true;
        return false;
      }


      /**
       * @brief vero se espressione matematica, che schifo
       * 
       * sicuramente esisterà un modo migliore per fare questa cosa
       * 
       * @param I 
       * @return 
       */
      static bool isMathExpression (Instruction* I) {
        auto ap = I->getOpcode();
          if ( ap== Instruction::Add  ) return true;
          if ( ap== Instruction::Mul  ) return true;
          if ( ap== Instruction::SDiv ) return true;
          if ( ap== Instruction::UDiv ) return true;
          if ( ap== Instruction::Sub  ) return true;
          if ( ap== Instruction::Shl  ) return true;
          if ( ap== Instruction::LShr ) return true;
          if ( ap== Instruction::ICmp ) return true;
          if ( ap== Instruction::AShr ) return true;
          return false;
        }



      /**
       * @brief definizione istruzione Loop-invariante
       * 
       * quanto:
       * - è una costante binaria isBinaryConstantInstStore() lo risolve
       * - tutte le definizioni degli operandi dell'istruzione sono definiti all'esterno del loop
       * - domina le uscite
       * 
       * @param I 
       * @return 
       */
      static bool isInvariant(Loop *L , Instruction* I , DominatorTree *DT ) { 
        SmallVector<BasicBlock*> allExit = printLoopINBlocksgetExit(L);
        return  isMathExpression(I) && ( 
          isBinaryConstantInstStore(I) || 
          ( hasUsesDefinedOutsideLoop(L,I)  && isExitDominator(I,DT,allExit) ) 
        ) ; 
      }

      /**
       * @brief Controlla convergenza
       * 
       * - è uguale a prima? si/no
       * - se no tiene il nuovo
       * 
       * @param toCheck 
       * @return 
       */
      static bool convCheck (SmallVector<Instruction*> toCheck) {
        static  SmallVector<Instruction*> convCheckDollar;
        if (toCheck.size()==0) return true; // prima volta 
        for(int i =0 ; i< toCheck.size() ;i++   ) 
          if (toCheck[i] != convCheckDollar[i] ) return false;
        return true;
      }


      /**
       * @brief Sto usando questa per creare il vettore di possibili invarianti
       * @param L 
       * @param app 
       * @return 
       */
      static SmallVector<Instruction*> CreateInvariantVec(Loop *L , DominatorTree *DT , SmallVector<Instruction*>app) {
        for (Loop::block_iterator BI = L->block_begin() ; BI != L->block_end();  ++BI) {
          BasicBlock &p = (**BI);
          for (BasicBlock::iterator I = p.begin() ; I != p.end() ; ++I) {
            Instruction &a=*I; 
            if ( isInvariant (L ,  &a , DT )  )  app.push_back(&a)  ; 
          } 
        } return app;
      }



      /**
       * @brief Ritorna vettore uscite e stampa
       * @param L 
       * @return 
       */
      static SmallVector<BasicBlock*> printLoopINBlocksgetExit(Loop *L) {
        SmallVector<BasicBlock*> allDom;
        for (Loop::block_iterator BI = L->block_begin() ; BI != L->block_end();  ++BI) {
          allDom.push_back(*BI); 
          BasicBlock &p = (**BI);  L->getExitBlocks( allDom );
          #ifdef vv
          outs() << p ; outs() << "\n";
          #endif
        }
        return allDom;
      }


      /**
       * @brief Tiene i passi di analisi per non rifarli ogni volta
       * @param AU 
       */
      virtual void getAnalysisUsage(AnalysisUsage &AU)const override{
        AU.addRequired<DominatorTreeWrapperPass>();
        AU.addRequired<LoopInfoWrapperPass>();
      }




      /**
      * Return true if the Loop is in the form that the LoopSimplify form
      * transforms loops to, which is sometimes called normal form.
      * outs() <<  L->isLoopSimplifyForm(); outs() << "\n"; 
      * 
      * If there is a preheader for this loop, return it.
      * outs() <<  *L->getLoopPreheader() ; outs() << "\n"; 
      * 
      * return Header if not null
      * outs() <<  *L->getHeader() ; outs() << "\n";
      * 
      * 
      * come si trovano le loop invariant?
      * si marchiano le istruzioni ( le metto in una pila o in una coda) se 
      * le definizioni dei suoi operandi sono fuori dal loop (nel preheader)
      * o costanti 
      * 
      * trova loop invariant
      * verifica che le condizioni di dominance siano soddisfatte
      * trova le istruzioni che sono solo reching definitions
      * si può usare il passo per le reaching definitions
      * 
      * sposta solo le reaching definitions
      */
      virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
        DominatorTree *DT = & getAnalysis<DominatorTreeWrapperPass>().getDomTree();
        SmallVector<Instruction*> lii;
        lii=CreateInvariantVec(L,DT,lii);
        //while (convCheck(lii) ) lii=CreateInvariantVec(L,DT,lii);
        checkCodeMotion(L->getLoopPreheader() , lii);
        return false;
      }

  };

    char LoopWalkPass::ID = 0;
    RegisterPass<LoopWalkPass> X("Loop-inv",
                                "Loop Invariant code motion");

} // anonymous namespace

