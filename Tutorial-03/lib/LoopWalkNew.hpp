#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopNestAnalysis.h>
#include <llvm/IR/PassManager.h>

class LoopWalkPassNew : public llvm::PassInfoMixin<LoopWalkPassNew>
{
  private:
    bool runOnLoop(llvm::Loop*);

  public:
    llvm::PreservedAnalyses run(llvm::LoopNest&, llvm::LoopAnalysisManager&, llvm::LoopStandardAnalysisResults&, llvm::LPMUpdater&);
};