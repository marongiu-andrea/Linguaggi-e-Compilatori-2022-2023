#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Value.h>
#include <vector>
#include <map>


using namespace llvm;

namespace {

/**nome in italiano per evitare conflitti
o lancio la funzione ricorsivamente, o mi serve una lista delle altre Invariant instruction
ovvero dei registri che contengono i risultati
*/
bool eLoopInvariante(Loop* L, Instruction* I, std::vector<Instruction*> invarianti){
  //gli operandi sono costanti?
  //sono delle istruzioni?
  //sono nella lista degli invarianti?
  //controllo per evitare che una branch o simili vengano considerate 
  for(User::op_iterator OP = I->op_begin(); OP != I->op_end(); ++OP){
    Value *oper = *OP;

    if(Instruction *operando_inst = dyn_cast<Instruction>(oper)){
      if(L->contains(operando_inst)){
        //l'operando è nel loop
        if(std::find(invarianti.begin(), invarianti.end(), operando_inst) == invarianti.end()){
          //E' un istruzione e non è invariante
          return false;
        }
      }
    }
  }

  //se sono qui, allora vuol dire che o sono costanti
  //oppure sono tutti loop invariant
  //binOP, shift, cast, select, cast, gateelemtptr
  if(I->isBinaryOp() || I->isShift() || I->isCast()){
    return true;
  }else{
    return false;
  }
}


class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU)const override{

    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    //Fase recupero iformazioni
    DominatorTree *DT = & getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    BasicBlock *preheader = L->getLoopPreheader();

    std::vector<Instruction*> istrInvarianti;

    //troviamo le istruzioni invarianti
    
    for(Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *LoopBlock=*BI;
      for(BasicBlock::iterator I = LoopBlock->begin(); I != LoopBlock->end(); ++I){
        Instruction *inst = dyn_cast<Instruction>(I);

        if(eLoopInvariante(L,inst,istrInvarianti)){
          istrInvarianti.push_back(inst);
          //quali sono gli altri controlli necessari

        }

      }
    }
    //vediamo che ho trovato
    for(int i=0; i<istrInvarianti.size(); i++){
      outs() << "Instruzione invariante: "<< *istrInvarianti[i] << " \n";
    }

    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

/*
  opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk test/Loop.ll -disable-output
*/