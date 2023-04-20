#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    if (!L)
      return false;

    DominatorTree *dt = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    LoopInfo *li = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    
    outs() << "\nLOOPPASS INIZIATO...\n"; 

    outs() << "\nLoop normalizzato " << ((L->isLoopSimplifyForm()) ? "true" : "false") << "\n";

    if (auto *preheader = L->getLoopPreheader()) {
      outs() << "\npreheader " << *preheader;
    }

    for (auto *block : L->getBlocks()) {
      for (auto &instr: *block) {
        if (instr.getOpcode() == Instruction::Sub) {
          outs() << "\ninstruction " << instr;

          outs() << "\n operands: ";
          for (auto op = instr.op_begin(); op != instr.op_end(); ++op) {
            outs() << "\n ";
            if (auto *instr = dyn_cast<Instruction>(*op)) {
              outs() << "\n " << *instr;
            }
          }
          outs() << "\nbasic block " << *block;
          outs() << "\n\n\n";
        }
      }
    }

    outs() << "\n";
    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

