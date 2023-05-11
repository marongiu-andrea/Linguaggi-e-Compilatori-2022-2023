#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/ValueTracking.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"

using namespace llvm;

namespace
{
  std::vector<Instruction *> instructionsToMove;

  bool isLoopInvariant(BinaryOperator *I, Loop *L);
  bool isLoopInvariantOperand(Value *op, Loop *L);

  bool isLoopInvariantOperand(Value *op, Loop *L)
  {
    outs() << "\tReaching definitions: " << *op << "\n";
    if (dyn_cast<ConstantInt>(op) || dyn_cast<Argument>(op))
    {
      outs() << "\t\tE' loop invariant, perchè è un'argomento o costante\n";
      return true;
    }
    else if (dyn_cast<PHINode>(op))
    {
      outs() << "\t\tNon è loop invariant, perchè è una phi node\n";
      return false;
    }
    else if (dyn_cast<Instruction>(op))
    {
      Instruction *instParent = dyn_cast<Instruction>(op);
      BasicBlock *parentBlock = instParent->getParent();

      outs() << "\t\tL'operando è un'istruzione, blocco: ";
      parentBlock->printAsOperand(outs(), false);
      outs() << "\n";
      bool isInLoop = L->contains(parentBlock);
      if (!isInLoop)
      {
        outs() << "\t\tL'istruzione non è dentro al loop,  è loop-invariant \n";
        return true;
      }
      else
      {
        outs() << "\t\tL'istruzione è dentro al loop,\n";
        BinaryOperator *binOpInstParent = dyn_cast<BinaryOperator>(instParent);
        if (binOpInstParent)
        {
          bool parentIsLoopInvariant = isLoopInvariant(binOpInstParent, L);
          if (parentIsLoopInvariant)
          {
            outs() << "\t\tL'istruzione padre è loop-invariant, quindi anche l'operando non dipende dal loop \n";
            return true;
          }
          else
          {
            outs() << "\t\tL'istruzione padre non è loop-invariant, quindi anche l'operando dipende dal loop \n";
            return false;
          }
        }
      }
    }
    outs() << " BOH \n";
    return true;
  }

  bool isLoopInvariant(BinaryOperator *I, Loop *L)
  {
    return (isLoopInvariantOperand(I->getOperand(0), L) && isLoopInvariantOperand(I->getOperand(1), L));
  }

  bool checkDominator(Loop *L, BinaryOperator *I, DominatorTree &DT)
  {
    BasicBlock *loopExitBlock = L->getExitBlock();
    for (auto *exitPred : predecessors(loopExitBlock))
    {
      if (!DT.dominates(I, exitPred))
      {
        // L'istruzione loop-invariant non domina un'uscita del loop
        return false;
      }
    }
    return true;
  }

  class LoopInvariantCodeMotion final : public LoopPass
  {
  public:
    static char ID;

    LoopInvariantCodeMotion() : LoopPass(ID) {}

    virtual void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<DominatorTreeWrapperPass>();
      AU.setPreservesAll();
      outs() << "getAnalysisUsage\n";
    }

    virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override
    {
      DominatorTree &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();

      outs()
          << "\nLOOPPASS INIZIATO...\n";
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
            if (isLoopInvariant(binOpInstruction, L))
            {
              if (checkDominator(L, binOpInstruction, DT))
              {
                outs() << "\t**ISTRUZIONE LOOP INVARIANT E DOMINANTE**\n";
                instructionsToMove.push_back(binOpInstruction);
              }
              else
                outs() << "\t**ISTRUZIONE LOOP INVARIANT NON DOMINANTE**\n";
            }

            else
              outs() << "\t**ISTRUZIONE LOOP VARIANT**\n";
          }
          outs() << "\n";
        }

        outs() << "\n\n";
      }
      outs() << "\n\n"
             << "INIZIO MOVIMENTO ISTRUZIONI\n";

      for (auto *I : instructionsToMove)
      {
        outs() << *I << "\n";
        I->moveBefore(L->getLoopPreheader()->getTerminator());
      }

      return false;
    }
  };

  char LoopInvariantCodeMotion::ID = 0;
  RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                                          "Loop Invariant Code Motion");
} // namespace
