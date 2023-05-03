#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>

using namespace llvm;

namespace {

class LoopWalkPass final : public LoopPass {
public:
  static char ID;

  LoopWalkPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOPPASS INIZIATO...\n";
    if (L->isLoopSimplifyForm()) {
      if (L->getLoopPreheader()) {
        BasicBlock *BB = L->getLoopPreheader();
        outs() << *BB << "\n";

        for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();
             ++BI) {
          BasicBlock *B = *BI;
          outs() << *B << "\n";
          outs()<<"INIZIO ANALISI\n";
          DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
          DT->print(outs());
        }
        for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end();
             ++BI) {
          BasicBlock *B = *BI;
          for (auto iter = B->begin(); iter != B->end(); ++iter) {
            Instruction &inst = *iter;
            if (inst.getOpcode() == Instruction::Sub) {
              ConstantInt *c1 = dyn_cast<ConstantInt>(inst.getOperand(0));
              ConstantInt *c2 = dyn_cast<ConstantInt>(inst.getOperand(1));
              if (!c1) {
                Value *opVal = inst.getOperand(0);
                Instruction *def = dyn_cast<Instruction>(opVal);
                BasicBlock* bb = def->getParent();
                bb->printAsOperand(outs(),false);
                /*outs() << "BasicBlock della definizione dell' istruzione sub:\n"
                       << *B;*/
              }
              if (!c2) {
                Value *opVal = inst.getOperand(1);
                Instruction *def = dyn_cast<Instruction>(opVal);
                /*outs() << "BasicBlock della definizione dell' istruzione sub:\n"
                       << *B;*/
                BasicBlock* bb = def->getParent();
                bb->printAsOperand(outs(),false);
              }
            }
          }
        }
      }
    }
    return false;
  }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("loop-walk", "Loop Walk");

} // anonymous namespace
