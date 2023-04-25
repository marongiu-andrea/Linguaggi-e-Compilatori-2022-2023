#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace
{
  void checkOperand(Value *op, Loop *L)
  {

    outs() << "\tOperando " << *op << "\n";
    // outs() << "\tContesto " << *(*op->getContext() << "\n";
    // transform this into a switch  statement
    if (dyn_cast<ConstantInt>(op))
    {
      // Non ha reaching definitions
      outs() << "\t\tE' una costante\n";
    }
    else if (dyn_cast<Argument>(op))
    {
      // Non ha reaching definitions
      outs() << "\t\tE' un argomento\n";
    }
    else if (dyn_cast<PHINode>(op))
    {
      // Non ha reaching definitions
      outs() << "\t\tE' un PHINode\n";
      PHINode *phiNode = dyn_cast<PHINode>(op);
      outs() << "\t\t\tNumero di incoming values: " << phiNode->getNumIncomingValues() << "\n";
      for (unsigned i = 0, e = phiNode->getNumIncomingValues(); i != e; ++i)
      {
        outs() << "\t\t\t\t" << i << " " << *(phiNode->getIncomingValue(i)) << "\n";
        // outs() << "\t\t\t\t" << i << " " << *(phiNode->getIncomingBlock(i)->) << "\n";
        outs() << "\t\t\t\t" << i << " get blokken: ";
        phiNode->getIncomingBlock(i)->printAsOperand(outs(), false);
        outs() << "\n";
        outs() << "\t\t\t\t il blocco è in loop: " << L->contains(phiNode->getIncomingBlock(i)) << "\n";
      }
    }
    else if (dyn_cast<Instruction>(op))
    {
      Instruction *inst = dyn_cast<Instruction>(op);
      BasicBlock *parentBlock = inst->getParent();

      outs() << "\tBasic Block della definizione: ";
      parentBlock->printAsOperand(outs(), false);
      outs() << "\n";
    }
    else
    {
      outs() << "\t\tE' un altro tipo di operando\n";
    }
  }

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
            checkOperand(Iter->getOperand(0), L);
            checkOperand(Iter->getOperand(1), L);
          }
          outs() << "\n";
        }

        outs() << "\n\n";
      }
      return false;
    }
  };

  char LoopInvariantCodeMotion::ID = 0;
  RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                                          "Loop Invariant Code Motion");
} // namespace
