#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <set>

using namespace llvm;

class LICMPass final : public LoopPass {

private:
  std::set<Instruction*> markedAsLoopInvariants;
  std::set<BasicBlock*>  exitBasicBlocks;
  std::set<Instruction*> hoistableInstructions;

  bool isIntructionLoopInvariant(Instruction*, Loop*);
  bool isOperandLoopInvariant(Value*, Loop*);
  bool isExitBasicBlock(BasicBlock*, Loop*);
  bool dominatesAllExitBlocks(Instruction*, DominatorTree*);
  bool isDeadOutOfLoop(Instruction*, Loop*);

public:
  static char ID;

  LICMPass() : LoopPass(ID) { }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
  virtual bool runOnLoop(Loop* L, LPPassManager& LPM) override;
};