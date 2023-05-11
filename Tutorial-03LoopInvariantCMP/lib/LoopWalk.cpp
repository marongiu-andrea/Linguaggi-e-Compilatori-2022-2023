#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ValueTracking.h>
//#include <llvm/Support/GenericLoopInfo.h>

//#include <llvm/ADT/SmallVector.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();

  //altri comandi:
  //setPreservesAll()
  //

  // DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  // da usare nel passo per richiedere il domTree, il codice prima non commentato setta il domtreewrapper come necessario
  // al passo

  // LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().g;

  }
  //SmallVector<Instruction> invariant;
  //Calcoliamo la lista dei bb del loop. La passiamo a isLoopInvariant, trovi il bb genitore dell'istruzione,
  //if genitore is in lista allora false e quindi non è loop invariant.

  //primitiva contains
  bool isLoopInvariant(Instruction &Inst, Loop *L, std::set<Instruction*> &VisitedInstrs) {
    if (VisitedInstrs.count(&Inst))
        return true;
    VisitedInstrs.insert(&Inst);
    for(auto *opIter = Inst.op_begin(); opIter != Inst.op_end(); ++opIter){
      Value *op = *opIter;
      
      //Se non e' una const
      if(Instruction *arg = dyn_cast<Instruction>(op))
      {
        if(L->contains(arg))//dichiarato dentro loop
        {
          //outs() << "arg is inside the loop: " << *arg << "\n";
            //if(arg == &Inst)
            //  return false;
            if(!isLoopInvariant(*arg,L,VisitedInstrs))
              return false;
        }else { // I is outside the loop
          //outs() << "arg is outside the loop: " << *arg << "\n";
          if (!isa<Constant>(op)) {
            return false;
          }
        }
        //outs() << "Istruzione che definisce: " << *op <<"\n";
        //outs() << "Basic Block dell'istruzione:\n" << *arg->getParent() << "\n";
      } else { // It is not an instruction
          //outs() << "op is a constant: " << *op << "\n";
          if (!isa<Constant>(op)) {
            return false;
          }
        }
    }
    return true;
  } 

  SmallVector<BasicBlock*> dominatesAllExits( DominatorTree &DT, Loop *L) {

    BasicBlock *dominator = nullptr;
    SmallVector<BasicBlock*> exits;
    SmallVector<BasicBlock*> dominators;
// Find the nearest common dominator of the exit block and all the blocks inside the loop
    for (auto *block : L->getBlocks()) {
      if (block != L->getHeader() && L->isLoopExiting(block))
        exits.push_back(block);
      
      if (dominator == nullptr) {
        // First time through the loop, set the dominator to the current block
        dominator = block;
      } else {
        // Update the dominator to be the nearest common dominator of the current block and the previous dominator
        dominator = DT.findNearestCommonDominator(dominator, block);
      }
    }

    //getExitingBlocks non viene trovato, abbiamo deciso di andare a creare la lista noi 


  for (auto *exit : exits) {
    if (DT.dominates(dominator, exit))
      dominators.push_back(dominator);
    
  }
  return dominators;
}



  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n"; 
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    SmallVector<Instruction*> Invariants;
    SmallVector<BasicBlock*> Dominators = dominatesAllExits(*DT,L);
    std::set<Instruction*> Visited;

    for (auto bb : Dominators){
      outs()<<"Dominator: " << bb<< "\n";
    }
    // LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    // verificare la forma normalizzata
    if (L->isLoopSimplifyForm()){
      outs() << "\nLoop in forma normalizzata\n";
      
      // controllo preheader
      BasicBlock *B1 = L->getLoopPreheader();
      outs() << "\nPreheader del loop: " <<  *B1 << "\n";

      // itero sui basic blocks del loop
      int i = 1;
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
        BasicBlock *BB = *BI;
        outs() << "\nBasic block n. " << i << ": " << *BB << "\n";
        i++;

        outs() << "Scrorrendo le istruzioni del BB: \n";
        for(auto InstIter = BB->begin(); InstIter != BB->end(); ++InstIter){
          Instruction &Inst = *InstIter;

          if(isLoopInvariant(Inst, L,Visited)){
            outs() << "E' loop invariant " <<Inst<< "\n";
            Invariants.push_back(&Inst);
          }else{
            outs() << "Non è loop invariant" <<"\n";
          }
        }
      }

          for (auto *inst : Invariants)
          {
            for (auto *block : Dominators)
            {
              if (inst->getParent() == block)
              {
                outs()<<"L'istruzione "<<*inst<<" is trova nel dominatore: "<< block<<"\n";
              }
            }
          }
      
      return true;
    }
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

