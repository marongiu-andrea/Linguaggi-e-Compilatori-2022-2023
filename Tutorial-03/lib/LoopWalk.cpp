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

/*
Funzione che controlla se l'istruzione non ha sideEffects (usata una sola volta)
O se domina tutti i blocchi del loop
il nome astruso è per evitare conflitti
E' da fare
*/
bool esSicuraToHost(Instruction* I){
  return false;
}
/*
std::vector<BasicBlock*> uscite(Loop* L){
  std::vector<BasickBlock*> uscite;
  if(L->isLoopSimplifyForm()){
    return L->getUniqueExitBlock();
  }else{
    for(Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *LoopBlock=*BI;

    }
    return NULL;
  }
}
*/
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
    bool dominaUscite=false;
    bool dominaUtilizzi=false;

    DominatorTree *DT = & getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    BasicBlock *preheader = L->getLoopPreheader();
    BasicBlock *ExitB;
    if(L->isLoopSimplifyForm()){
      ExitB = L->getUniqueExitBlock();
    }
    std::vector<Instruction*> istrInvarianti;
    std::vector<Instruction*> toMoove;

    //devo ciclare sul preheadre, in preorder sul dominator tre
    //devo controllare che il blocco non sia in un inner loop o fuori L
    //per ogni istruzione devo controllare che sia LoopInvariant e safeto Hoist
    //se si, sposto l'istruzione nel preheader

    
    for(Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *LoopBlock=*BI;
      if(DT->dominates(LoopBlock,ExitB)){
        dominaUscite=true;
      }else{
        dominaUscite=false;
      }
      for(BasicBlock::iterator I = LoopBlock->begin(); I != LoopBlock->end(); ++I){
        Instruction *inst = dyn_cast<Instruction>(I);
        if(eLoopInvariante(L,inst,istrInvarianti)){
          istrInvarianti.push_back(inst);
          //quali sono gli altri controlli necessari
          if(dominaUscite){
            toMoove.push_back(inst);
            //cerchiamo gli usi e vediamo se dominiamo quei bloccho
            dominaUtilizzi=true;
            for(Value::use_iterator USO = inst->use_begin(); USO != inst->use_end(); ++USO){
              if(Instruction *UserI = dyn_cast<Instruction>(*USO)){
                if(DT->dominates(LoopBlock,UserI->getParent())){
                }else{
                  dominaUtilizzi = false;
                }
              }
            }
            if(dominaUtilizzi){
              printf("Domino anche gli utilizzi\n");
            }
          }
        }

      }
    }
    //vediamo che ho trovato
    for(int i=0; i<toMoove.size(); i++){
      outs() << "Instruzione invariante: "<< *toMoove[i] << " \n";
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