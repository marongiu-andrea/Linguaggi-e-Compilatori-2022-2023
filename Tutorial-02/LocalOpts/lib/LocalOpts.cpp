#include "LocalOpts.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <tuple>

using namespace llvm;

std::tuple<Value*, ConstantInt*> LocalOpts::getVariableAndConstantOperandsFromBinaryInstruction(Instruction& instr)
{
    Value* variable = nullptr;
    ConstantInt* constant = nullptr;

    if (instr.isBinaryOp())
    {
        Value* left = instr.getOperand(0);
        Value* right = instr.getOperand(1);

        variable = left;
        constant = dyn_cast<ConstantInt>(right);

        if (constant == nullptr)
        {
            variable = right;
            constant = dyn_cast<ConstantInt>(left);
        }
    }

    return {variable, constant};
}

bool LocalOpts::runOnFunction(Function& function)
{
    bool transformed = false;

    for (auto& bb : function)
    {
        if (runOnBasicBlock(bb))
        {
            transformed = true;
        }
    }

    return transformed;
}

PreservedAnalyses LocalOpts::run(llvm::Module& module, llvm::ModuleAnalysisManager& manager)
{
    for (auto& function : module)
    {
        if (runOnFunction(function))
        {
            return PreservedAnalyses::none();
        }
    }

    return PreservedAnalyses::none();
}

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo()
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
                    [](StringRef name, ModulePassManager& passManager,
                       ArrayRef<PassBuilder::PipelineElement>) -> bool {
                        if (name == "transform")
                        {
                            passManager.addPass(TransformPass());
                            return true;
                        }
                        else if (name == "algebraic-identity")
                        {
                            passManager.addPass(AlgebraicIdentityPass());
                            return true;
                        }
                        else if (name == "strength-reduction")
                        {
                            passManager.addPass(StrengthReductionPass());
                            return true;
                        }
                        else if (name == "multi-instr-opt")
                        {
                            passManager.addPass(MultiInstrOptPass());
                            return true;
                        }

                        // TODO: Implementare gli stub per
                        // Algebraic Identity
                        // Strength Reduction
                        // Multi-instruction Operations
                        return false;
                    });
            } // RegisterPassBuilderCallbacks
    }; // struct PassPluginLibraryInfo
}
