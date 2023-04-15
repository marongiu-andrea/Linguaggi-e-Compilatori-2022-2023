#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n";
    outs() << "ESERCIZIO 1 e 2\n";
    if((*L).isLoopSimplifyForm())
    {
      if(BasicBlock *preheader = (*L).getLoopPreheader())
      {
        outs()<<"PreHeader: "<<*preheader;
        for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
        {
          BasicBlock *B = *BI;
          //outs()<<*B<<"\n";
          for(auto Iter = (*B).begin(); Iter != (*B).end(); ++Iter )
          {
            if( (*Iter).getOpcode() == Instruction::Sub ) // controllo se l'operazione che viene effettuata è una MUL
            {
              Value *Operand1 = (*Iter).getOperand(0);
              Value *Operand2 = (*Iter).getOperand(1);
              ConstantInt *C = dyn_cast<ConstantInt>(Operand1);
              ConstantInt *D = dyn_cast<ConstantInt>(Operand2);

              if(C == nullptr)
              {
                Instruction *reference = dyn_cast<Instruction>(Operand1);
                outs()<<*reference<<"\n";
                outs()<<*(*reference).getParent()<<"\n";
              }
              if(D == nullptr)
              {
                Instruction *reference = dyn_cast<Instruction>(Operand2);
                outs()<<*reference<<"\n";
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

