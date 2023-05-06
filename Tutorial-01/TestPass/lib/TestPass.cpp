#include <algorithm>
#include <bits/ranges_algo.h>
#include <concepts>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

using namespace llvm;

namespace
{
    class TestPass final : public PassInfoMixin<TestPass>
    {
      public:
        PreservedAnalyses run([[maybe_unused]] Module& module, ModuleAnalysisManager&)
        {
            outs() << "Passo di test per il corso di Linguaggi e Compilatori"
                   << "\n";

            std::for_each(module.begin(), module.end(), [&module](Function& function) {
                std::string argSize(std::to_string(function.arg_size()));
                unsigned int callToFunctionsInSameModuleCount = 0;

                outs() << "Funzione: " << function.getName() << "\n";
                outs() << "Numero di argomenti: " << (function.isVarArg() ? argSize + "+*" : argSize) << "\n";

                std::for_each(function.begin(), function.end(), [&module, &callToFunctionsInSameModuleCount](BasicBlock& bb) {
                    callToFunctionsInSameModuleCount += std::count_if(bb.begin(), bb.end(), [&module](Instruction& instr) {
                        CallInst* callInstr = dyn_cast_or_null<CallInst>(&instr);

                        return callInstr != nullptr &&
                               callInstr->getCalledFunction()->getParent()->getModuleIdentifier() == module.getModuleIdentifier();
                    });
                });

                outs() << "Numero di chiamate a funzioni nello stesso modulo: " << callToFunctionsInSameModuleCount << "\n";
                outs() << "Numero di basic block: " << function.size() << "\n";
                outs() << "Numero di istruzioni: " << function.getInstructionCount() << "\n";
                outs() << "----------"
                       << "\n";
            });

            return PreservedAnalyses::all();
        }
    };
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "TestPass",
        .PluginVersion = LLVM_VERSION_STRING,
        .RegisterPassBuilderCallbacks = [](PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager& MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                    if (Name == "test-pass")
                    {
                        MPM.addPass(TestPass());
                        return true;
                    }
                    return false;
                });
        }};
}
