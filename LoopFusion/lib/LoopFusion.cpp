#include "LoopFusion.h"

#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>

using namespace llvm;

PreservedAnalyses LoopFusion::run(Function& function, FunctionAnalysisManager& fam)
{
    LoopInfo& loopInfo = fam.getResult<LoopAnalysis>(function);
    SmallVector<Loop*, 4> loops = loopInfo.getLoopsInPreorder();

    if (loops.size() == 0)
    {
        outs() << "Non ci sono loop nella funzione.\n";
    }
    else if (loops.size() == 1)
    {
        outs() << "Nella funzione c'è un solo loop. Impossibile dare informazioni sull'adiacenza.\n";
    }
    else
    {
        for (unsigned int i = 0; i < loops.size() - 1; i++)
        {
            unsigned int j = i + 1;
            Loop* loopi = loops[i];
            Loop* loopj = loops[i + 1];
            BasicBlock* loopiExitBlock = loopi->getExitBlock();
            BasicBlock* loopjPreheader = loopj->getLoopPreheader();

            if (loopiExitBlock == nullptr)
            {
                outs() << "loop[i] ha più di un'uscita. Impossibile dare informazioni sull'adiacenza.\n";
            }
            else if (loopjPreheader == nullptr)
            {
                outs() << "loop[j] non è in forma normalizzata. Impossibile dare informazioni sull'adiacenza.\n";
            }
            else
            {
                if (loopiExitBlock == loopjPreheader)
                {
                    outs() << "loop[" << i << "] è adiacente a loop[" << j << "].\n";
                }
                else
                {
                    outs() << "loop[" << i << "] non è adiacente a loop[" << j << "].\n";
                }
            }
        }
    }

    return PreservedAnalyses::all();
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "CustomLoopFusion",
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
                    [](StringRef name, FunctionPassManager& passManager, ArrayRef<PassBuilder::PipelineElement>) -> bool {
                        if (name == "custom-loopfusion")
                        {
                            passManager.addPass(LoopFusion());
                            return true;
                        }

                        return false;
                    });
            }};
}
