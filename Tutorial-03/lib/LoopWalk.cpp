#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n";
    outs() << "ESERCIZIO 1 e 2\n";
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree(); // ottengo il DominatorTree
    LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

    //controllo che il loop sia in forma semplificata
    if((*L).isLoopSimplifyForm())
    {
      //ottengo il preheader se esiste
      if(BasicBlock *preheader = (*L).getLoopPreheader())
      {
        //stampo il preheader
        outs()<<"PreHeader: "<<*preheader;
        for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
        {
          BasicBlock *B = *BI;
          //outs()<<*B<<"\n"; //stampa di tutti i basic block commentata per non intasare l'output
          for(auto Iter = (*B).begin(); Iter != (*B).end(); ++Iter )
          {
            if( (*Iter).getOpcode() == Instruction::Sub ) // controllo se l'operazione che viene effettuata è una SUB
            {
              //otteniamo gli operandi e controlliamo se sono delle costanti
              Value *Operand1 = (*Iter).getOperand(0);
              Value *Operand2 = (*Iter).getOperand(1);
              ConstantInt *C = dyn_cast<ConstantInt>(Operand1);
              ConstantInt *D = dyn_cast<ConstantInt>(Operand2);

              //se il primo operando non è costante
              if(C == nullptr)
              {
                //ottengo l'istruzione di definizione dell'operando
                Instruction *reference = dyn_cast<Instruction>(Operand1);
                outs()<<"\nIstruzione della definizione dell'operando nella sub: \n";
                outs()<<*reference<<"\n";
                outs()<<"\nBlocco in cui sta la definizione: ";
                outs()<<*(*reference).getParent()<<"\n";
              }
              //se il secondo operando non è costante
              if(D == nullptr)
              {
                //ottengo l'istruzione di definizione dell'operando
                Instruction *reference = dyn_cast<Instruction>(Operand2);
                outs()<<"Istruzione della definizione dell'operando nella sub: \n";
                outs()<<*reference<<"\n";
                outs()<<"\nBlocco in cui sta la definizione: ";
                outs()<<*(*reference).getParent()<<"\n";
              }
            }
          }
        }
      }
    }

    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

