#include "LoopWalkNew.hpp"
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopNestAnalysis.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Transforms/Scalar/LoopPassManager.h>

using namespace llvm;

bool LoopWalkPassNew::runOnLoop(Loop* loop)
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

PreservedAnalyses LoopWalkPassNew::run(LoopNest& loop, LoopAnalysisManager& manager, LoopStandardAnalysisResults& r, LPMUpdater& u)
{
    for (auto* l : loop.getLoops())
    {
        if (runOnLoop(l))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}

extern "C" llvm::PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "LocalOpts",
        .PluginVersion = LLVM_VERSION_STRING,
        .RegisterPassBuilderCallbacks =
            [](PassBuilder& PB) {
                PB.registerPipelineParsingCallback(
                    // CREO LO STUB per il mio TestPass
                    //
                    // RICORDA: Posso usare (coi relativi Pass Managers)
                    // -------------------------------------------------
                    // ModulePass
                    // CallGraphSCCPass
                    // FunctionPass
                    // LoopPass
                    // RegionPass
                    // BasicBlockPass
                    [](StringRef name, LoopPassManager& passManager, ArrayRef<PassBuilder::PipelineElement>) -> bool {
                        if (name == "loop-walk")
                        {
                            passManager.addPass(LoopWalkPassNew());
                            return true;
                        }

                        return false;
                    });
            }};
}