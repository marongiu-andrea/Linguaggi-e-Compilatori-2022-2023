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
        Value *op = opIter->get();
      //se non e' un PHINode
      if(isa<PHINode>(op) || isa<BranchInst>(op))
        return false;

      //Se non e' una const
      if (Instruction *arg = dyn_cast<Instruction>(op)) {
        if (L->contains(arg)) // dichiarato dentro loop
        {
          if(!isLoopInvariant(*arg,L,VisitedInstrs))
            return false;
        }
      }
      else{
        if (!isa<Constant>(op))
          return false;
      }
    }
    return true;
  }

  SmallVector<BasicBlock*> getExitDominators( DominatorTree &DT, Loop *L) {

    BasicBlock *dominator = nullptr;
    SmallVector<BasicBlock*> exits;
    SmallVector<BasicBlock *> dominators;
    // Find the nearest common dominator of the exit block and all the blocks inside the loop
    for (auto *block : L->getBlocks()) {
      if (block != L->getHeader() && L->isLoopExiting(block))
        exits.push_back(block);

      if (dominator == nullptr)
        dominator = block;
      else
        dominator = DT.findNearestCommonDominator(dominator, block);
    }

    for (auto *exit : exits) {
      if (DT.dominates(dominator, exit))
        dominators.push_back(dominator);
    }
  return dominators;
}

  bool dominatesUseBlocks(DominatorTree &DT, Instruction *inst){
    for (auto Iter = inst->user_begin(); Iter != inst->user_end(); ++Iter) {
      if (!DT.dominates(inst, dyn_cast<Instruction>(*Iter))) {
        return false;
      }
    }
    return true;
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n";
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    SmallVector<Instruction*> Invariants;
    SmallVector<BasicBlock*> Dominators = getExitDominators(*DT,L);
    std::set<Instruction*> Visited;
    SmallVector<Instruction*> Movable;

    for (auto bb : Dominators){
      outs()<<"Dominator: " << bb<< "\n";
    }
    // LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    // verificare la forma normalizzata
    if (L->isLoopSimplifyForm()){
      outs() << "\nLoop in forma normalizzata\n";

      // controllo preheader
      BasicBlock *preHeader = L->getLoopPreheader();
      outs() << "\nPreheader del loop: " <<  *preHeader << "\n";

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
            outs()<<"L'istruzione "<<*inst<<" is trova nel dominatore: "<< block <<"\n";

            if (dominatesUseBlocks(*DT, inst)){
              Movable.push_back(inst);
            }
          }
        }
      }

      for (auto elem : Movable)
      {
        outs()<<"Trovata istruzione movable: "<<*elem<<"\n";
        elem->moveBefore(preHeader->getTerminator());
      }

      return true;
    }
    return false;
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

