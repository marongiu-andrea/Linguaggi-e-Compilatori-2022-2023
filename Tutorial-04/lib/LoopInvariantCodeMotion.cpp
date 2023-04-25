#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace
{

  class LoopInvariantCodeMotion final : public LoopPass
  {
  public:
    static char ID;

    LoopInvariantCodeMotion() : LoopPass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
    }

    virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override
    {
      outs() << "\nLOOPPASS INIZIATO...\n";
      if (L->isLoopSimplifyForm())
      {
        outs() << "FORMA NORMALIZZATA\n";
      }
      BasicBlock *BB = L->getLoopPreheader();
      if (BB)
      {
        outs() << "PREHEADER " << *BB << "\n";
      }
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
      {
        BasicBlock *B = *BI;

        outs() << "Basic Block: ";
        B->printAsOperand(outs(), false);
        outs() << "\n";

        for (auto Iter = B->begin(); Iter != B->end(); ++Iter)
        {
          outs() << *Iter;

          BinaryOperator *binOpInstruction = dyn_cast<BinaryOperator>(Iter);
          if (binOpInstruction)
          {
            outs() << "\t###Vediamo dentro###\n";
            // TODO: capire se una istruzione è loop invariant
            printBasicBlock(Iter->getOperand(0));
            printBasicBlock(Iter->getOperand(1));
          }
          outs() << "\n";
        }

        outs() << "\n\n";
      }
      return false;
    }

    void printBasicBlock(Value *op)
    {
      outs() << "\tOperando " << *op << "\n";
      if (dyn_cast<ConstantInt>(op))
      {
        outs() << "\t\tE' una costante\n";
      }
      else
      {
        Instruction *inst = dyn_cast<Instruction>(op);
        BasicBlock *parentBlock = inst->getParent();

        outs() << "\tBasic Block della definizione: ";
        parentBlock->printAsOperand(outs(), false);
        outs() << "\n";
      }
    }
  };

  char LoopInvariantCodeMotion::ID = 0;
  RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                                          "Loop Invariant Code Motion");

} // anonymous namespace
