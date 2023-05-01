#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Value.h>
#include <llvm/Analysis/ValueTracking.h>



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

/*funzione per un blocco se domina tutte le uscite
o almeno dovrebbe
idea: trova le uscite, e sfrutta il c++, quindi fare qualcos'altro nel passo principale.(non ottimizziamo
prima del tempo)
*/
bool dominaUsciteF(BasicBlock *BB,
                  DominatorTree *DT, 
                  std::vector<BasicBlock*> blocchiExit){
  //non è per nulla ottimizzato ma va be
  for(int i = 0; i< blocchiExit.size(); ++i){
    if(!DT->dominates(BB,blocchiExit[i]))
      return false;
  }
  return true;

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
    //bool dominaUscite=false;
    bool dominaUtilizzi=false;

    DominatorTree *DT = & getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    BasicBlock *preheader = L->getLoopPreheader();

    std::vector<BasicBlock*> blocchiExit;
    std::vector<Instruction*> istrInvarianti;
    std::vector<Instruction*> toMoove;

    //devo ciclare sul preheadre, in preorder sul dominator tre
    //devo controllare che il blocco non sia in un inner loop o fuori L
    //per ogni istruzione devo controllare che sia LoopInvariant e safeto Hoist
    //se si, sposto l'istruzione nel preheader

    //controllo se il successore di un blocco sia dentro il loop
    for(Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *LoopBlock=*BI;
      if(L->isLoopExiting(LoopBlock)){
        blocchiExit.push_back(LoopBlock);
      }
    }

    //per gentile concessione di ChatGPT esploro l'albero in depth firts
    /*df_begin è un iteratore che esiste proprio per esplorare un Dominetor tree depth first
      funziona perche il Dominator Tree implementa il template DominatorTreeBase che restituisce
      vari oggetti/info in base alle richieste... due settimane a cercare un modo per farlo...
      3 minuti a ChadGPT per spiegarmelo 
    */
    for(auto I = df_begin(DT->getRootNode()); I != df_end(DT->getRootNode()); ++I){
      BasicBlock *LoopBlock = I->getBlock();
      //domina tutte le uscite?
      if(dominaUsciteF(LoopBlock,DT,blocchiExit)){
        //outs() << "Sono il basicblock che domina le uscite"<< *BB <<" \n";
        //controlliamo le istruzioni
        for(BasicBlock::iterator Iter = LoopBlock->begin(); Iter != LoopBlock->end(); ++Iter){
          Instruction *inst = dyn_cast<Instruction>(Iter);
          dominaUtilizzi = true;
          //è loop invariant?
          if(eLoopInvariante(L,inst,istrInvarianti)){
            istrInvarianti.push_back(inst);
            //Domina tutti i blocchi che la usano ?
            for(Value::use_iterator USO = inst->use_begin(); USO != inst->use_end(); ++USO){
              if(Instruction *UserI = dyn_cast<Instruction>(*USO)){
                if(DT->dominates(LoopBlock,UserI->getParent())){
                }else{
                  dominaUtilizzi = false;
                }
              }
            }
            //se e solo se domino tutti gli utilizzi
            if(dominaUtilizzi){
              //si puoi essere spostata
              toMoove.push_back(inst);
            }


          }
        }
      }

    }
    
    //vediamo che ho trovato
    for(int i=0; i<toMoove.size(); i++){

      outs() << "Instruzione invariante: "<< *toMoove[i] << " \n";

      toMoove[i]->moveBefore(preheader->getTerminator());
    }

    return false;
    //E infine mi siedo osservando l'orizzonte di un loop grato per le mie azioni  
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

/*
  opt -enable-new-pm=0 -load ./libLoopWalk.so -loop-walk test/Loop.ll -disable-output
*/