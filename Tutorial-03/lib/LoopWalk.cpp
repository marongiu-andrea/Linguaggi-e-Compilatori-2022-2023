#include </usr/include/llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Instruction.h>
using namespace llvm;

class LoopWalkPass final : public LoopPass
{
  public:
    static char ID;

    LoopWalkPass() :
        LoopPass(ID) {}

    void getAnalysisUsage(AnalysisUsage& AU) const override
    {
    }

    bool runOnLoop(Loop* L, LPPassManager& LPM) override
    {
        outs() << "\nLOOPPASS INIZIATO...\n";
        if (L->isLoopSimplifyForm())
            outs() << "Il Loop e' in forma normalizzata\n";
        else
            outs() << "Il loop non e' in forma normalizzata\n";

        BasicBlock* preheader = L->getLoopPreheader();
        if (preheader)
            outs() << "Il loop ha un preheader: " << *preheader;
        else
            outs() << "Il loop non ha preheader\n";

        outs() << "Questi sono i basic blocks che formano il loop:\n";
        ArrayRef<BasicBlock*> loopBlocks = L->getBlocks();
        for (auto block : loopBlocks)
            outs() << *block;

        outs() << "Queste sono le sottrazioni con istruzioni come operandi:\n";
        for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI)
        {
            for (auto instr = (*BI)->begin(); instr != (*BI)->end(); ++instr)
                if (instr->getOpcode() == Instruction::Sub)
                {
                    outs() << "La sottrazione" << *instr << " ha le seguenti istruzioni come operandi: \n";
                    printInstruction(*instr);
                }
        }
        return false;
    }

    void printInstruction(const Instruction& instr)
    {
        Instruction* op1 = dyn_cast<Instruction>(instr.getOperand(0));
        Instruction* op2 = dyn_cast<Instruction>(instr.getOperand(1));
        if (op1)
        {
            outs() << *op1 << " definita nel blocco: ";
            op1->getParent()->printAsOperand(outs());
            outs() << "\n";
        }

        if (op2)
        {
            outs() << *op2 << " definita nel blocco: ";
            op2->getParent()->printAsOperand(outs());
            outs() << "\n";
        }
    }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");
