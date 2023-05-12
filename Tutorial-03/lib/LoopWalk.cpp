#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace
{

  class LoopWalkPass final : public LoopPass
  {
  public:
    static char ID;

    LoopWalkPass() : LoopPass(ID) {}

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
          if (Iter->getOpcode() == Instruction::Sub)
          {
            outs() << *Iter << " ###QUESTA E UNA SUB###\n";

            PHINode *binOpLeft = dyn_cast<PHINode>(Iter->getOperand(0));
            PHINode *binOpRight = dyn_cast<PHINode>(Iter->getOperand(1));

            if (binOpLeft)
            {
              printBasicBlock(Iter->getOperand(0));
            }
            if (binOpRight)
            {
              printBasicBlock(Iter->getOperand(1));
            }
          }
          else
          {
            outs() << *Iter << "\n";
          }
        }
        outs() << "\n\n";
      }
      return false;
    }

    void printBasicBlock(Value *op)
    {
      outs() << "\tOperando " << *op << "\n";
      Instruction *inst = dyn_cast<Instruction>(op);
      BasicBlock *parentBlock = inst->getParent();
      outs() << "\tBasic Block della definizione: ";
      parentBlock->printAsOperand(outs(), false);
      outs() << "\n";
    }
  };

  char LoopWalkPass::ID = 0;
  RegisterPass<LoopWalkPass> X("loop-walk",
                               "Loop Walk");

} // anonymous namespace
