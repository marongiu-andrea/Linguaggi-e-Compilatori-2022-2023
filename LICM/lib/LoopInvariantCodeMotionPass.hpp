#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instruction.h>
#include <set>

class LoopInvariantCodeMotionPass : public llvm::LoopPass
{
  private:
    void getLoopInvariantInstructionsAndExitingBlocks(llvm::Loop*, std::set<llvm::Instruction*>&, std::set<llvm::BasicBlock*>&);

    bool isInstructionLoopInvariant(llvm::Loop* L, std::set<llvm::Instruction*> const& liInstrs, llvm::Instruction const& i);

    void getHoistableInstructions(llvm::Loop*, llvm::DominatorTree const&, std::set<llvm::Instruction*> const&, std::set<llvm::BasicBlock*> const&, std::set<llvm::Instruction*>&);

  public:
    static char ID;

    LoopInvariantCodeMotionPass();

    void getAnalysisUsage(llvm::AnalysisUsage&) const override;

    bool runOnLoop(llvm::Loop*, llvm::LPPassManager&) override;
};