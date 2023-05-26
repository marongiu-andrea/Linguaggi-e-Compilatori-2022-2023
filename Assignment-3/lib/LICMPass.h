#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>

using namespace llvm;

class LICMPass final : public LoopPass {
public:
  static char ID;

  LICMPass() : LoopPass(ID) { }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;

  virtual bool runOnLoop(Loop* L, LPPassManager& LPM) override;
};