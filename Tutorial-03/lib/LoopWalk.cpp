#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n"; 

    outs() << "\n"; 

    outs() << *L;  outs() << "\n"; 

    // outs() <<  L->isLoopSimplifyForm(); outs() << "\n"; 
    // outs() <<  *L->getLoopPreheader() ; outs() << "\n"; 
    // outs() <<  *L->getHeader() ; outs() << "\n";

    for (Loop::block_iterator BI = L->block_begin() ; BI != L->block_end();  ++BI) {
      BasicBlock &p = (**BI);
      //outs() << p ; outs() << "\n";
      for (BasicBlock::iterator I = p.begin() ; I != p.end() ; ++I) {
        //outs() << *I; outs() << "\n";
        // usa un dynamic cast e un if 
        // per trovare la sub %12  come esercizio loop walk pass pagina 12
        switch (I->getOpcode())
          case Instruction::Sub: {
          outs() << "here a sum perhaps to be canceled\n";
          outs() << *I; outs() << "\n";

          // segui gli usi della SUB per cercare il basic block da cui derivano queste istruzioni


          break;
        }

      }

      
    }



    return false; 
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk",
                             "Loop Walk");

} // anonymous namespace

