#include <llvm/IR/PassManager.h>
#include <llvm/IR/Constant.h>

using namespace llvm;

class LoopFusionPass final : public PassInfoMixin<LoopFusionPass> 
{
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &)
};