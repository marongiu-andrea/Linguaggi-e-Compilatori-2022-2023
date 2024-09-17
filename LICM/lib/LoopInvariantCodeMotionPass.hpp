#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Use.h>
#include <set>

class LoopInvariantCodeMotionPass : public llvm::LoopPass
{
  private:
    llvm::Loop* currentLoop;
    llvm::DominatorTree* dominatorTree;
    std::set<llvm::Instruction*> loopInvariantInstructions;
    std::set<llvm::BasicBlock*> loopExitingBlocks;
    std::set<llvm::Instruction*> hoistableInstructions;

    void analyze(llvm::Loop*);

    void searchForLoopInvariantInstructionsAndLoopExitingBlocks();
    void searchForHoistableInstructions();

    std::set<llvm::Instruction*> getLoopInvariantInstructions(llvm::BasicBlock*);

    bool isLoopInvariant(const llvm::Instruction&);
    bool isLoopInvariant(const llvm::Use&);
    bool isLoopExiting(const llvm::BasicBlock*);

    bool dominatesAllExits(const llvm::Instruction*);
    bool isDeadAfterLoop(const llvm::Instruction*);

    void printAnalysisResult();

  public:
    static char ID;

    LoopInvariantCodeMotionPass();

    void getAnalysisUsage(llvm::AnalysisUsage&) const override;

    bool runOnLoop(llvm::Loop*, llvm::LPPassManager&) override;
};