#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/ADT/SmallVector.h>
#include <set>


using namespace llvm;

class LICMPass final : public LoopPass {

private:
  Loop* loop;
  DominatorTree* dominatorTree;
  std::set<Instruction*>   markedAsLoopInvariants;
  std::set<Instruction*>   hoistableInstructions;
  SmallVector<BasicBlock*> exitingBasicBlocks;


  bool isIntructionLoopInvariant(Instruction*);
  bool isOperandLoopInvariant(Value*);
  bool dominatesAllExitingBlocks(Instruction*);
  bool isDeadOutOfLoop(Instruction*);

  void getLoopInvariantInstructions();
  void getHoistableInstructions();

  void moveInstructions();

  void printLoopInvariantInstructions();
  void printHoistableInstructions();

public:
  static char ID;

  LICMPass() : LoopPass(ID) { }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
  virtual bool runOnLoop(Loop* L, LPPassManager& LPM) override;
};