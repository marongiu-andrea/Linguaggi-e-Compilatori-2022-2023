#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Instructions.h>
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
  bool isLoopInvariant(Instruction &Inst, Loop *L) {

    for(auto *opIter = Inst.op_begin(); opIter != Inst.op_end(); ++opIter){
        Value *op = opIter->get();
      //se non e' un PHINode
      if(isa<PHINode>(Inst) || isa<BranchInst>(Inst))
        return false;
      //Se non e' una const
      if (Instruction *arg = dyn_cast<Instruction>(op)) {
        if (L->contains(arg)){// dichiarato dentro loop
          if(!isLoopInvariant(*arg,L))
            return false;
        }
      }
    }
    return true;
  }

  bool dominatesExits(Instruction *inst, DominatorTree &DT, Loop *L) {

    SmallVector<BasicBlock*> exits;

    for (auto *block : L->getBlocks()) {
      if (block != L->getHeader() && L->isLoopExiting(block))
        exits.push_back(block);
    }

    for (auto *exit : exits) {
      if (!DT.dominates(inst->getParent(), exit))
        return false;
    }

  return true;
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
    SmallVector<Instruction*> Movable;

    // verificare la forma normalizzata
    if (L->isLoopSimplifyForm()){
      outs() << "\nLoop in forma normalizzata\n";

      // itero sui basic blocks del loop
      int i = 1;
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
        BasicBlock *BB = *BI;
        outs() << "\nBasic block n. " << i << ": " << *BB << "\n";
        i++;

        outs() << "Scrorrendo le istruzioni del BB: \n";
        for(auto InstIter = BB->begin(); InstIter != BB->end(); ++InstIter){
          Instruction &Inst = *InstIter;

          if(isLoopInvariant(Inst, L)){
            outs() << Inst << "E' loop invariant " << "\n";
            Invariants.push_back(&Inst);
          }
          else
            outs() << "Non è loop invariant" <<"\n";
        }
      }

      for (auto *inst : Invariants)
      {
        if (dominatesExits(inst,*DT ,L ) && dominatesUseBlocks(*DT, inst))
          Movable.push_back(inst);
      }

      BasicBlock *preHeader = L->getLoopPreheader();
      for (auto elem : Movable)
      {
        outs()<<"Trovata istruzione movable: "<<*elem<<"\n";
        elem->moveBefore(&preHeader->back());
      }
      return true;
    }
    return false;
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

