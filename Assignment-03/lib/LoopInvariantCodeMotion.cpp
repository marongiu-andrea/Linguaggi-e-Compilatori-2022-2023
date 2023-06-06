#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <map>
#include <vector>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll(); //indico di preservare i risutati di questo passo di analisi
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>(); 
    //queste due direttive mi specificano di dover eseguire prima i passi di Dominance e LoopInfo prima di procedere ad eseguire questo passo, perche' i risultati
    //di tali passi verranno utilizzati in questo passo di analisi
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n"; 
    //verifico se il loop e' nella forma normale/semplificata
    if (L->isLoopSimplifyForm()) {
      outs() << "Il Loop e' nella forma normale!\n\n";
    }

    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree(); //contiene il dominance tree che ottengo dal passo di analisi precedente
    std::vector<BasicBlock*> BBvector; //vettore di BB che contengono istruzioni di salto fuori dal loop
    std::vector<Instruction*> Movablevector; //vettore di Instruction* che contiene le istruzioni che devo andare a spostare nel preheader

    //prendo il blocco di preheader
    BasicBlock *preheader = L->getLoopPreheader();
    BasicBlock *actualBB;


    //MARK: trovo i basic block che hanno una uscita del loop
    for(Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      if (L->isLoopExiting(*BI)){
        BBvector.push_back(*BI);
      }
    }

    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) { //scorro i BB del loop
      actualBB = *BI;
      for (auto &instr : *actualBB) { //scorro le istruzioni all'interno del singolo BB
        //se l'istruzione individuata e' loop invariant, allora procedo ad analizzare le condizioni per la code motion
        if(isLoopInvariant(&instr, L)){
          outs() << "La seguente istruzione e' Loop Invariant: ";
          instr.print(outs());
          outs() << "\n";
          //verifico se la definizione individuata domina tutti gli usi all'interno del loop
          if(!dominateUses(&instr,L,DT)){
            continue;//l'istruzione non e' movable
          }
          outs() << "La seguente istruzione domina tutti i suoi usi: ";
          instr.print(outs());
          outs() << "\n";
          if(!dominateExits(&instr,L,DT,&BBvector)){
            continue;
          }
          outs() << "La seguente istruzione domina tutte le uscite del loop: ";
          instr.print(outs());
          outs() << "\n";

          outs() << "\nSono pronto a portare fuori dal loop l'istruzione!\n";
          LLVMContext& C = instr.getContext();
          MDNode* N = MDNode::get(C, MDString::get(C, "hoistable")); //creo il metadata node da attaccare all'istruzione attuale per marcarla come movable
          instr.setMetadata("HOIST", N);
          Movablevector.push_back(&instr);
        }
      }
    }

    //scorro il loop per spostare le istruzioni taggate come movable

    Instruction *last_preheader = preheader->getTerminator(); //mi posiziono alla fine del preheader per procedere ad inserire le istruzioni movable
    Instruction *instr;
    for(auto iter = Movablevector.begin(); iter != Movablevector.end(); ++iter){
        instr = *iter;
        instr->moveBefore(last_preheader);
    }
    return false;
  }

  void checkOperand(Value* operand) {
    if (!dyn_cast<ConstantInt>(operand)) { //se l'operando che ho appena ricevuto non e' una costante, allora lo provo a castare ad istruzione e stamparne il BB di appartenenza
      Instruction *instr = dyn_cast<Instruction>(operand);
      if (instr) { //il cast ad istruzione e' andato a buon fine
        BasicBlock *B = instr->getParent();
        B->printAsOperand(outs(),false);
        outs() << "\n";
      }
    }
  }

  bool isLoopInvariant(Instruction *instr, Loop *L) {
    //outs() << "Invocato isLoopInvariant con la seguente istruzione: ";
    //instr->print(outs());
    //outs() << "\n";
    
    if (!instr->isBinaryOp() || instr->getOpcodeName() == "phi" || instr->getOpcodeName() == "br"){
      return false;
    }
    BasicBlock *parentBB; //BB al quale appartiene l'istruzione considerata
    ConstantInt *const_pointer; //operando castato a costante
    Instruction* instr_pointer; //operando castato ad istruzione
    Argument* arg; //operando castatato ad argomento di una funzione
    Value* operand;
    bool flag = true; //all'inizio il mio flag e' settato a true
    for(int i=0;flag && i<instr->getNumOperands();i++){
      const_pointer = dyn_cast<ConstantInt>(instr->getOperand(i));
      if(!const_pointer){ //se non e' una costante, allora provo a castarla ad istruzione
        instr_pointer = dyn_cast<Instruction>(instr->getOperand(i)); //MARK: per ora gestisco solo il caso delle istruzioni che vengono castate a binaryoperator
        if(instr_pointer) { //se e' una istruzione non br e non phi
          //caso in cui l'operando deve essere definito fuori dal loop
          if (!instr_pointer->isBinaryOp() || instr_pointer->getOpcodeName() == "phi" || instr_pointer->getOpcodeName() == "br"){
            return false;
          }
          parentBB = instr_pointer->getParent();
          if(!L->contains(parentBB)){
            continue;
          }
          //caso in cui verifico se l'operando e' una istruzione gia' marcata come loop invariant
          if(isLoopInvariant(instr_pointer,L)){
            continue;
          }
          flag=false; //se e' un BinaryOperato che non ha soddisfatto nessuna delle due condizioni precedenti, allora e' di sicuro NON LOOP-INVARIANT
        }
        else {
          arg = dyn_cast<Argument>(instr->getOperand(i)); //provo a castare a function parameter
          if(arg){
            continue; //se non e' ne' una costante ne' un BinaryOperator, allora e' un function parameter, che quindi e' LOOP-INVARIANT
          }
          else{ //MARK: caso in cui non sia nulla di tutto quello che ho controllato fino a questo momento (istruzione, costante, argomento di una funzione)
            outs() << "Operando della attuale istruzione non riconosciuto, per sicurezza ritorno false\n";
            return false;
          }
        }
      }
      else{
        continue; //se e' una costante ragiono sul prossimo operando
      }
    }
    return flag;
  }

  bool dominateUses(Instruction *instr, Loop *L, DominatorTree *DT) {
    BasicBlock *def_block = instr->getParent();
    /*
    if(!DT->dominates(instr,def_block)){ //se la definizione non domina gli usi nel suo blocco, allora so' gia' che non va bene
      outs() << "C'e' qualcosa che non va' in questo controllo iniziale\n";
      return false;
    }
    */
   bool flag=true;
    for (auto iter = instr->user_begin(); flag && iter != instr->user_end(); ++iter){ //scorro tutti gli usi della variabile LHS dell'istruzione
      Instruction *uso = dyn_cast<Instruction>(*iter);
      if(!DT->dominates(instr,uso)){//se il cast e' andato a buon fine, allora estraggo il BB di appartenenza dell'uso
        flag=false;
      }
    }
    return flag; //se gli usi sono dominati tutti dalla definizione, allora ritorno true
  }

  bool dominateExits(Instruction *instr, Loop *L, DominatorTree *DT, std::vector<BasicBlock*> *vect) {
    BasicBlock *parentBB = instr->getParent();
    bool flag=true;

    for(auto iter = vect->begin(); flag && iter != vect->end(); ++iter) {
      if(!DT->dominates(parentBB,*iter)){
        flag=false;
      }
    }
    return flag;
  }

};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace
