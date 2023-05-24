#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/Dominators.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/ADT/SetVector.h>
#include <llvm/ADT/MapVector.h>
#include <llvm/ADT/DenseMap.h>
#include <map>


using std::multimap;
using std::pair;

using namespace llvm;

namespace {


class LoopInvariantCodeMotionPass final : public LoopPass {
public:
  static char ID;

  LoopInvariantCodeMotionPass() : LoopPass(ID) {}

  //ritorna true solo se vengono soddisfatti i criteri per la loop invariant
  bool isOperandLoopInvariant (Value *V, Loop *L, SetVector<Instruction*> const &invariant, Instruction *inst, multimap<Instruction*, Instruction*> &invariantDependences){

    if (isa<Constant>(V) || isa<Argument>(V)) //operando costante o argomento di funzione sono loop invariant
      return true;

    if (Instruction *I = dyn_cast<Instruction>(V)){ //è un'istruzione di qualche tipo
        if (!L->contains(I)) //esterna al loop le reaching definitions sono fuori dal loop è loop invariant
        return true;
    
      if (L->contains(I) && !isa<PHINode>(I)) //interna al loop, se non è un PHI node ho un'unica reaching def dentro al loop 
        if (invariant.contains(I)){  //se è già inserita in invariant è loop invariant
          invariantDependences.insert(pair<Instruction*, Instruction*>(inst, I)); //inserisco la dipendenza
          return true;
        }
    }    
    return false;
  }

  //ritorna true se l'istruzione rispetta i requisiti per la Code Motion
  bool isHoistable (Instruction *I, Loop *L, DominatorTree const &dt, SmallVector<BasicBlock*> const &exitBB){

    outs() << "\nControllo Istruzione: ";
    I->print(outs());

    bool domExit = true; //rimane true solo se il blocco dell'istruzione domina tutte le uscite
    for (BasicBlock* block : exitBB){ 
      if (!domExit)
        break;
      domExit = dt.dominates(I->getParent(), block);
    }

    bool isDead = true; //rimane true solo se non ha usi fuori dal loop
    for (auto BI : exitBB){ //per ogni uscita genero un vettore di blocchi esterni al loop e controllo se l'istruzione è usata in uno di essi
      SmallVector<BasicBlock*> outOfLoop;
      dt.getDescendants(BI, outOfLoop);
      for (BasicBlock* block : outOfLoop)
        if (I->isUsedInBasicBlock(block))
          isDead = false;
      }

    bool domUse = true; //rimane true solo se il blocco dell'istruzione domina tutti gli usi
    for (Use& U : I->uses()){
        domUse = dt.dominates(I->getParent(), U);  
        if (!domUse)
          break;
    }

    bool onlyVarDef = true;     //rimane true se sto definendo una variabile per cui non ho altre definizioni
    for (Use& U : I->uses()){   //controllo se tra gli usi ho un PHI node nel ciclo, in tal caso sto definendo una variabile per cui ho altre definizioni
      if (PHINode *phi = dyn_cast<PHINode>(U.getUser())){ 
        if (L->contains(phi))
          onlyVarDef = false;
      }
    }
    outs() << "\nRISULTATO CONTROLLI.\ndomExit isDead domUse onlyVarDef: " << domExit << isDead << domUse << onlyVarDef;

    if ((domExit || isDead) && domUse && onlyVarDef){

      outs() << " OK\nIsruzione candidata.\n";

      return true;
    }
    outs() << " KO\nIsruzione non candidata.\n";

    return false;
  }
  

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOP PASS INIZIATO...\n";

    outs() << "\nIl loop è:\n";
    L->print(outs());
    outs() << "\n";

    if (!L->isLoopSimplifyForm()) { //se il loop non è in forma normale non ho il singolo entry point e non posso spostare nulla
      outs() << "\nLOOP NON IN FORMA NORMALE\n";
      return false;
    }

    DominatorTree& dt = getAnalysis<DominatorTreeWrapperPass>().getDomTree();

    outs() << "\nIl Dominator Tree è:\n";
    dt.print(outs());
    outs() << "\n";


    //strutture dati per gestire le varie fasi del processo   

    SmallVector<BasicBlock*> exitBB; //uscite del loop
    SetVector<Instruction*> invariant; //istruzioni loop invariant
    SetVector<Instruction*> hoistable; //istruzioni candidate alla Code Motion
    SmallVector<Instruction*> toHoist; //istruzioni da spostare nel preheader
   
    //permette di tracciare le dipendenze di una istruzione per essere spostata. Ogni coppia <key, value> rappresenta <istruzione, dipendenza>
    multimap<Instruction*, Instruction*> invariantDependences; 
                                                              


    L->getExitBlocks(exitBB);  //memorizzo le uscite del loop

    outs() << "\nGli Exit Blocks sono:\n";
    for (BasicBlock *B : exitBB){
      B->print(outs());
      outs() << "\n";
    }

    BasicBlock *preheader = L->getLoopPreheader();
    BasicBlock *header = L->getHeader();
    Instruction* preheader_terminator = preheader->getTerminator();

    outs() << "\nIl preheader è:\n";
    preheader->print(outs());
    outs() << "\n";

    outs() << "\nIl preheader terminator è:\n";
    preheader_terminator->print(outs());
    outs() << "\n";

    outs() << "\nL'header è:\n";
    header->print(outs());
    outs() << "\n";

    outs() << "\n\nCERCO LE ISTRUZIONI LOOP INVARIANT...\n\n";

    //cerco istruzioni loop invariant
    for (auto BI = L->block_begin(); BI != L->block_end(); ++BI){
      BasicBlock *block = *BI;
      for (auto inst_iter = block->begin(); inst_iter != block->end(); ++inst_iter){
        Instruction *inst = dyn_cast<Instruction>(inst_iter);

        //se l'istruzione è binaria posso valutare la Code Motion
        if (inst->isBinaryOp()){
          Value* op1 = inst->getOperand(0);
          Value* op2 = inst->getOperand(1);

          //verifico se entrambi gli operandi sono loop invariant e, nel caso, inserisco l'istruzione
          //inoltre passo alla funzione anche l'istruzione e la map per le dipendenze
          if (isOperandLoopInvariant (op1, L, invariant, inst, invariantDependences) && isOperandLoopInvariant (op2, L, invariant, inst, invariantDependences)){

            outs() << "\nTrovata istruzione Loop Invariant:\n";
            inst->print(outs());
            outs() << "\n";

            invariant.insert(inst);
          }
        }
      }
    }
    outs() << "\n\nCERCO LE ISTRUZIONI CANDIDATE ALLA CODE MOTION...\n\n";

    //trovo istruzioni candidate alla Code Motion tra le loop invariant
    for (Instruction* inst : invariant)
      if (isHoistable(inst, L, dt, exitBB))
        hoistable.insert(inst);
    

    outs() << "\n\nCERCO LE ISTRUZIONI DA SPOSTARE...\n\n";
    //eseguo DFS accettando solo i blocchi del loop e le istruzioni binarie
    for (auto node = GraphTraits<DominatorTree*>::nodes_begin(&dt); node != GraphTraits<DominatorTree*>::nodes_end(&dt); ++node){
      BasicBlock *block = node->getBlock();
      if (L->contains(block)){
        for (auto inst_iter = block->begin(); inst_iter != block->end(); ++inst_iter){
          Instruction &inst = *inst_iter;
          if (!inst.isBinaryOp()){ 
            continue;
          }

          if (hoistable.contains(&inst)){ //verifico che l'istruzione sia tra le candidate

            bool noDependences = true;    //diventa false se ha delle dipendenze
            bool dependencesToHoist = true; //diventa false se almeno una delle sue dipendenze non è stata spostata

            auto range = invariantDependences.equal_range(&inst); //trovo tutte le coppie con key=inst

            for (auto iter = range.first; iter != range.second; ++iter){  //se il range non è vuoto, ho delle dipendenze ed entro nel ciclo 
              noDependences = false;
              if (hoistable.contains(iter->second)){ //se la dipendenza è in hoistable è perchè non è stata inserita in toHoist, non posso spostare l'istruzione
                dependencesToHoist = false;
                break;
              }
            }

            //se non ha dipendenze oppure sono già inserite in toHoist spostarle
            if (noDependences || dependencesToHoist){

              outs() << "\nTrovata istruzione da spostare:\n";
              inst.print(outs());
              outs() << "\n";

              toHoist.push_back(&inst); //inerisco l'istruzione in toHoist e la rimuovo da hoistable
              hoistable.remove(&inst);
            }          
            invariantDependences.erase(&inst);
          }
        }
      }       
    }
    outs() << "\n\nSPOSTO LE ISTRUZIONI...\n\n";

    //sposto le istruzioni nel preheader
    for(auto iter = toHoist.begin(); iter != toHoist.end(); ++iter){
        Instruction* inst = *iter;
        inst->moveBefore(preheader_terminator);
    }
    outs() << "\nLOOP PASS FINITO\n";
    return true;
  }
};
      
  


char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loopInvariantCodeMotion",
                             "loopInvariantCodeMotion");

}


