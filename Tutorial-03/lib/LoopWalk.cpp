#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/BasicBlock.h>

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

    bool runOnLoop(Loop* loop, LPPassManager& LPM) override
    {
        BasicBlock* preHeader = loop->getLoopPreheader();

        outs() << "Loop in forma normalizzata? " << loop->isLoopSimplifyForm() << "\n";

        if (preHeader != nullptr)
        {
            outs() << "Il loop ha un pre-header.\n";
            outs() << "Istruzioni del pre-header:\n";

            for (auto& instr : *preHeader)
            {
                instr.printAsOperand(outs(), false);

                outs() << ", tipo: " << instr.getOpcode() << "\n";

                outs() << "Operandi: ";

                for (auto& operand : instr.operands())
                {
                    operand->printAsOperand(outs(), false);

                    outs() << " ";
                }

                outs() << "\n";
            }
        }

        outs() << "Basic block del loop:\n";

        for (auto* bb : loop->getBlocks())
        {
            outs() << bb << "\n";

            for (auto& instr : *bb)
            {
                instr.printAsOperand(outs(), false);

                outs() << ", tipo: " << instr.getOpcode() << "\n";

                outs() << "Operandi: ";

                for (auto& operand : instr.operands())
                {
                    operand->printAsOperand(outs(), false);

                    outs() << " ";
                }

                outs() << "\n";

                if (instr.getOpcode() == Instruction::Sub)
                {
                    outs() << "Ho trovato l'istruzione SUB.\n";

                    outs() << "Operandi:\n";

                    for (auto& operand : instr.operands())
                    {
                        operand->printAsOperand(outs(), false);

                        outs() << "\n";

                        Instruction* opDefInstr = dyn_cast<Instruction>(operand);

                        if (opDefInstr == nullptr)
                        {
                            outs() << "L'operando è una costante.\n";
                        }
                        else
                        {
                            outs() << "L'operando non è una costante.\n";

                            outs() << "Questo operando è definito all'istruzione " << opDefInstr << "\n";

                            outs() << "Questa istruzione sta nel basic block " << opDefInstr->getParent() << "\n";
                        }
                    }

                    outs() << "##########\n";
                }

                outs() << "----------\n";
            }
        }

        return false;
    }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");
