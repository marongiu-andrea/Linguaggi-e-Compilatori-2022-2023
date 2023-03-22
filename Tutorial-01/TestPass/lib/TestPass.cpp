#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;


void PrintData([[maybe_unused]] Module& M)
{
    outs() << "Passo di test per il corso di Linguaggi e Compilatori"
        << "\n";
    
    // Functions
    int numFunctions = 0;
    for(auto it = M.begin(); it != M.end(); ++it)
    {
        Function& func = *it;
        FunctionType& funcType = *func.getFunctionType();
        
        if(func.isDeclaration())
            outs() << "> Dichiarazione: ";
        else
            outs() << "> Funzione: ";
        
        outs() << func.getName().data() << ", Num arg: " << funcType.getNumParams();
        if(funcType.isVarArg())
            outs() << "+*";
        outs() << '\n';
        
        if(func.isDeclaration())
            continue;
        
        // Basic blocks
        int numBlocks = 0;
        int numInstr = 0;
        int numCalls = 0;
        for(auto it = func.begin(); it != func.end(); ++it)
        {
            BasicBlock& block = *it;
            
            // Instructions
            for(auto it = block.begin(); it != block.end(); ++it)
            {
                Instruction& instr = *it;
                
                // NOTE: CallInst type in dyn_cast needs to not be a pointer
                // Also note that dynamic_cast doesn't work because Instruction
                // is not polymorphic
                if(CallInst* callInstr = dyn_cast<CallInst>(&instr))
                {
                    Function& called = *callInstr->getCalledFunction();
                    outs() << "    > Chiamata a funzione: " << called.getName() << '\n';
                    
                    // Only count the functions defined in the same scope
                    if(!called.isDeclaration())
                        ++numCalls;
                }
                
                ++numInstr;
            }
            
            ++numBlocks;
        }
        
        outs() << "  Num istr: " << numInstr << '\n';
        outs() << "  Num chiamate nello stesso modulo: " << numCalls << '\n';
        outs() << "  Num BB: " << numBlocks << '\n';
        ++numFunctions;
    }
    
    outs() << "Num funzioni: " << numFunctions << '\n';
}


namespace {
    
    class TestPass final : public PassInfoMixin<TestPass> {
        public:
        PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
            
            PrintData(M);
            
            return PreservedAnalyses::all();
        }
    }; // class TestPass
    
} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "TestPass",
        .PluginVersion = LLVM_VERSION_STRING,
        .RegisterPassBuilderCallbacks =
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                                               [](StringRef Name, ModulePassManager &MPM,
                                                  ArrayRef<PassBuilder::PipelineElement>) -> bool {
                                                   if (Name == "test-pass") {
                                                       MPM.addPass(TestPass());
                                                       return true;
                                                   }
                                                   return false;
                                               });
        } 
    };
}
