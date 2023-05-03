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


  enum blockType{
      ENTRY = 0,
      EXIT = 1,
      REGULAR = 2,
  };

  struct basicBlockDeps{
      BasicBlock* blockRef;
      BitVector genSet;
      BitVector killSet;
      DenseMap<int,SmallVector<int>> NonSeparableDFASet;
  };

  struct basicBlockProps{
      enum blockType bType;
      BasicBlock* block;
      BitVector bbInput;
      BitVector bbOutput;
      BitVector genSet;
      BitVector killSet;
      DenseMap<int,SmallVector<int>> checkLHS;
      SmallVector<BasicBlock*>predBlocks;
      SmallVector<BasicBlock*>succBlocks;
  };


  int domainSize;
  BitVector boundaryConditions;
  BitVector initConditions;
  bool meetOp;
  bool passDir;
  SmallVector<BasicBlock*> poTraversal;
  SmallVector<BasicBlock*> rpoTraversal;
  DenseMap<BasicBlock*,SmallVector<BasicBlock*>> dominanceRelationMap;
  DenseMap<BasicBlock*,basicBlockProps*> dFAResult;
  DenseMap<BasicBlock*,basicBlockProps*> dataFlowHash;
  SmallVector<BasicBlock*> basicBlocks;
  DenseMap<BasicBlock*,int> domainMap;
  DenseMap<int,BasicBlock*> revDomainMap;
  DenseMap<BasicBlock*,basicBlockDeps*> bbSets;


namespace {


class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}


  void populateDepSets(DenseMap<BasicBlock*,basicBlockDeps*> bbSets)   {
    DenseMap<BasicBlock*,basicBlockDeps*>::iterator it = bbSets.begin();
    while(it != bbSets.end())  {
      struct basicBlockDeps* temp = bbSets[it->first];
      struct basicBlockProps* temp2 = dataFlowHash[it->first];
      temp2->genSet = temp->genSet;
      temp2->killSet = temp->killSet;
      temp2->checkLHS = temp->NonSeparableDFASet;
      it++;
    }
  }



  BitVector MeetOpf(SmallVector<BitVector> inputVectors)  {
    BitVector out_vector = inputVectors[0];
    for(int i = 1; i < (int)inputVectors.size(); ++i)  {
      if(meetOp )  out_vector &= inputVectors[i];
      else  out_vector |= inputVectors[i]; 
    }
    return out_vector;
  }



  void applyTransferFunction(struct basicBlockProps* basicBlock) {
    BitVector killSet = basicBlock->killSet;
    DenseMap<int,SmallVector<int>>::iterator it = basicBlock->checkLHS.begin();
    while(it != basicBlock->checkLHS.end())  {
      if(killSet[it->first])   {
        SmallVector<int>LHS = it->second;
        for(int i : LHS)   {
          if(basicBlock->bbOutput[i])   killSet.reset(it->first);
          else  killSet.set(it->first);
        }
      }
      it++;
    }
    if(passDir)  {
      basicBlock->bbInput = killSet.flip();
      basicBlock->bbInput &= basicBlock->bbOutput;
      basicBlock->bbInput |= basicBlock->genSet;
    }
    else {
      basicBlock->bbOutput = killSet.flip();
      basicBlock->bbOutput &= basicBlock->bbInput;
      basicBlock->bbOutput |= basicBlock->genSet;
    }
  }



 void getConnectedBlocks(BasicBlock* BB, struct basicBlockProps* basicBlock) {
   for(BasicBlock* predblock: predecessors(BB)) basicBlock->predBlocks.push_back(predblock);
   for(BasicBlock* succblock: successors(BB)) basicBlock->succBlocks.push_back(succblock);
 }



 void buildBasicBlockInfo(Function &F)  {
  BitVector empty(domainSize, false);
  for(BasicBlock &BB : F) {
    struct basicBlockProps* bbProp = new basicBlockProps();
    bbProp->block = &BB;
    bbProp->bbInput = empty;
    bbProp->bbOutput = empty;
    bbProp->genSet = empty;
    bbProp->killSet = empty;
    getConnectedBlocks(&BB,bbProp);

    if(&BB == &F.getEntryBlock())   bbProp->bType = ENTRY;
    else   bbProp->bType = REGULAR;

    for(Instruction &II: BB)  { Instruction *I = &II;
    if(dyn_cast<ReturnInst>(I)) bbProp->bType = EXIT;
    }
    dataFlowHash[&BB] = bbProp;
   }
   DenseMap<BasicBlock*,basicBlockProps*>::iterator it = dataFlowHash.begin();
   struct basicBlockProps* temp;
   while(it != dataFlowHash.end())  {
     temp = dataFlowHash[it->first];
     if(passDir)  {
       if(temp->bType == EXIT)   temp->bbOutput = boundaryConditions; 
       temp->bbInput = initConditions;
     }
     else   {
       if(temp->bType == ENTRY) temp->bbInput = boundaryConditions;

       temp->bbOutput = initConditions;
     }
     ++it;
   }
  }



  void genTraversalOrder(Function &F)  {
    using namespace std;
    SmallVector<BasicBlock*> rpoStack;
    po_iterator<BasicBlock*> start = po_begin(&F.getEntryBlock());
    po_iterator<BasicBlock*> end = po_end(&F.getEntryBlock());
    while(start != end)   {
      poTraversal.push_back(*start);
      rpoStack.push_back(*start);
      ++start;
    }
    while(!rpoStack.empty())  {
        rpoTraversal.push_back(rpoStack.back());
        rpoStack.pop_back();
    }
  }



  void executeDataFlowPass(Function &F,DenseMap<BasicBlock*,basicBlockDeps*> bbSets ) {
    bool hasConverged = false;
    DenseMap<BasicBlock*,BitVector> prevOutputs;
    int iter = 0;
    buildBasicBlockInfo(F);
    genTraversalOrder(F);
    populateDepSets(bbSets);
    SmallVector<BasicBlock*> blockTraverse = (! passDir) ? rpoTraversal : poTraversal;
    while(!hasConverged) {
      for(BasicBlock* BB : blockTraverse) {
        prevOutputs[BB] = dataFlowHash[BB]->bbOutput;
        SmallVector<BitVector> meetVectors;
        if(passDir)  {
          for(SmallVector<BasicBlock*>::iterator b = dataFlowHash[BB]->succBlocks.begin(); b != dataFlowHash[BB]->succBlocks.end(); ++b) 
          { meetVectors.push_back(dataFlowHash[*b]->bbInput);  }
        } 
        else {
          for(SmallVector<BasicBlock*>::iterator f = dataFlowHash[BB]->predBlocks.begin(); f != dataFlowHash[BB]->predBlocks.end(); ++f) {
            meetVectors.push_back(dataFlowHash[*f]->bbOutput); }
        }
        if(!meetVectors.empty())  {
          BitVector meet = MeetOpf(meetVectors);
          if(passDir ) { dataFlowHash[BB]->bbOutput = meet; }
          else { dataFlowHash[BB]->bbInput = meet; } 
        }
        applyTransferFunction(dataFlowHash[BB]);
      }
      hasConverged = true;
      for(DenseMap<BasicBlock*,BitVector>::iterator it = prevOutputs.begin();it != prevOutputs.end();++it) { 
        if((dataFlowHash[it->first]->bbOutput) != it->second) { hasConverged = false; break; }
      }
      ++iter;
    }
  }


    void genTFDependencies(Function &F, SmallVector<BasicBlock*> domain) {
      BitVector empty((int)domain.size(), false);
      int vectorIdx = 0;
      for(BasicBlock* bb : domain)  {
        domainMap[bb] = vectorIdx;
        revDomainMap[vectorIdx] = bb;
        ++vectorIdx;
      }
      for(BasicBlock &BB : F) {
        struct basicBlockDeps* bbSet = new basicBlockDeps();
        bbSet->blockRef = &BB;
        bbSet->genSet = empty;
        bbSet->killSet = empty;
        bbSet->genSet.set(domainMap[bbSet->blockRef]); 
        bbSets[&BB] = bbSet;
      }
    }



  static bool NotInvariant(Instruction* I) { 
    return ( I->isBinaryOp() && (isSafeToSpeculativelyExecute(I)) && (!I->mayReadFromMemory())
     && (!isa<LandingPadInst>(I))) ; 
  }



  void buildDominanceMap(DenseMap<BasicBlock*,SmallVector<BasicBlock*>> &dmap,DenseMap<BasicBlock*,basicBlockProps*> dFAHash) {
    DenseMap<BasicBlock*,basicBlockProps*>::iterator it = dFAHash.begin();
    while(it != dFAHash.end())  {
      struct basicBlockProps* temp = dFAHash[it->first];
      SmallVector<BasicBlock*> domSet;
      for(int k = 0; k < (int)temp->bbInput.size(); k++)   if(temp->bbInput[k]) domSet.push_back(revDomainMap[k]); 
      dmap[temp->block] = domSet;
      it++;
    }
  }



  /**
   * @brief Sono store binarie,con due costanti. 
   * @param I 
   * @return bool
   */
  static bool isBinaryConstantInstStore ( Instruction* I ) {
    const APInt *k;
    // espressione  binaria, due costanti
    if (I->isBinaryOp()  &&  static_cast<StoreInst*>( &*I )
        &&  dyn_cast<ConstantInt>(I->getOperand(0))  && 
        dyn_cast<ConstantInt>(I->getOperand(1))
     )return true;
    return false;
  }



  /**
   * @brief Sto usando questa per creare il vettore di possibili invarianti
   * @param L 
   * @param app 
   * @return 
   */
  static SmallVector<Instruction*> CreateInvariantVec(Loop *L ) {
    SmallVector<Instruction*>app2; 
    for (Loop::block_iterator BI = L->block_begin() ; BI != L->block_end();  ++BI) {
      BasicBlock &p = (**BI);
      for (BasicBlock::iterator I = p.begin() ; I != p.end() ; ++I) {
        Instruction &a=*I; outs() << *I;
        if (! NotInvariant (&a)  ) {outs() << " I invariant "; app2.push_back(&a)  ;  }  ; outs() << "\n";
      }
    }
    return app2;
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
      BasicBlock &p = (**BI);   L->getExitBlocks( allDom );
      #ifndef vebose
      outs() << p ; outs() << "\n";
      #endif
    }
    return allDom;
  }



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
    
    SmallVector<BasicBlock*> allExit = printLoopINBlocksgetExit(L);

    bool dominaUtilizzi=false;

    DominatorTree *DT = & getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    BasicBlock *preheader = L->getLoopPreheader();

    outs() << "\nLICM  INIZIATO...\n"; 


    /**
     * Forma normale o loop naturale
     * L’header domina tutti i nodi nel loop
     * 
     * Il loop naturale di un back edge è il più piccolo insieme
     * di nodi che includa testa e coda del back edge e non
     * abbia predecessori al di fuori di questo insieme (esclusi i
     * predecessori dell’header).
     * 
     */

    if (L->isLoopSimplifyForm()) {outs() << "Forma normale o loop naturale \n"; }
    else {outs() << "Forma NON normale o loop NON naturale \n"; }
    //outs()  << "Il loop ha nodi --> " << L->getLoopDepth() << "\n";;
    //outs()<< "Preheader \n" <<  *L->getLoopPreheader() ; outs() << "\n";  
    //outs()<< "Header \n" <<  *L->getHeader() ; outs() << "\n";

    outs()<< *L << "stampa loop\n";

    SmallVector<Instruction*>app; //outs()  << "size  -------- " <<  app.size() << " ---\n";
    app=CreateInvariantVec(L);
    outs()  << "Creato vettore di undominant invariant Instruction"  << "\n";

    // controlla che siano reaching definition
    // ora dovrei cercare tutte le istruzioni che dominano e spostarle nel preheader
    //app=controllaUsee(L,app);

    // sposta tutte le costanti


    return false;

  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

