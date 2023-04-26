#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID)
  {
  }

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override 
  { 
  }

  virtual bool runOnLoop(Loop* L, LPPassManager& LPM) override
  {
    auto& os = outs();

    if(L->isLoopSimplifyForm())
      os << "Loop is simplify form\n";
    
    os << "#----------------------#\n";

    BasicBlock* preheader = L->getLoopPreheader();
    if(preheader)
    {
      os << "Preheader:\t";
      preheader->print(os);
    }

    os << "#----------------------#\n";
    
    for(auto bi : L->blocks())
    {
      os << "BasicBlock:\n";
      bi->print(os);
      os << "\n";
    }
 
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk","Loop Walk");

} // anonymous namespace

