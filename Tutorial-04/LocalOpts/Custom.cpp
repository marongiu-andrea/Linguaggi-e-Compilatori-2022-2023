#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>

using namespace llvm;

#include "llvm/ADT/SmallVector.h"


// Funzione appoggio per ordinare gli insiemi di loop CFG-equivalenti secondo dominanza.
struct less_than_key
{
  DominatorTree &DT;
  less_than_key(DominatorTree &D) : DT(D) {}

  inline bool operator() (const Loop* loop1, const Loop* loop2)
  {
    outs() << "Coppia\n";
    outs() << *loop1 << "\n";
    outs() << *loop2 << "\n";
    return (DT.dominates(loop1->getHeader(), loop2->getHeader()));
  }
};


llvm::PreservedAnalyses TransformPass::run([[maybe_unused]] llvm::Function &F, llvm::FunctionAnalysisManager &AM) {
  
  // Analisi utili nel seguito.
  auto &LI = AM.getResult<LoopAnalysis>(F);
  auto &DT = AM.getResult<DominatorTreeAnalysis>(F);
  auto &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);


  llvm::SmallVector<Loop*> LoopVector = LI.getLoopsInPreorder();  // Vector dei loop.
  std::map<Loop*, bool> LoopsAssigned;                            // Lookup per loop assegnati ad insiemi.

  for (auto iter_loop = LoopVector.begin(); iter_loop != LoopVector.end(); ++iter_loop) {
    Loop *L = *iter_loop;

    if (L->isLoopSimplifyForm()) {
      outs() << "Loop in forma std:\n";
      outs() << *L << "\n";

      LoopsAssigned[L] = false;
    }
    // Ricordarsi di eliminare loop non in normal form.
  }

  std::cout << "LoopVector size: " << LoopVector.size() << std::endl;
  
  std::vector<std::vector<Loop*>> CFGEquivalents;   // Vector degli insiemi di loop CFG-equivalent.

  for (auto iter_i = LoopVector.begin(); iter_i != LoopVector.end(); ++iter_i) {
    Loop *L1 = *iter_i;

    if (!LoopsAssigned[L1]) {
      std::vector<Loop*> equiv_set;
      equiv_set.push_back(L1);
      LoopsAssigned[L1] = true;

      for (auto iter_j = ++iter_i; iter_j != LoopVector.end(); ++iter_j) {
        Loop *L2 = *iter_j;

        if (!LoopsAssigned[L2]) {

          if (DT.dominates(L1->getHeader(), L2->getHeader()) &&
            PDT.dominates(L2->getHeader(), L1->getHeader())) {
            equiv_set.push_back(L2);
            LoopsAssigned[L2] = true;
          }
        }
      }

      if (equiv_set.size() > 1) {
        CFGEquivalents.push_back(equiv_set);
      }
    }
  }

  outs() << "CFGEquivalents size: " << CFGEquivalents.size() << "\n";

  outs() << "Insiemi da riordinare:\n";
  for (auto i = CFGEquivalents.begin(); i != CFGEquivalents.end(); ++i) {
    std::vector<Loop*> V = *i;
    std::reverse(V.begin(), V.end());

    for (auto j = V.begin(); j != V.end(); ++j) {
      Loop *L = *j;
      outs() << *L << "\n";
    }

    std::sort(V.begin(), V.end(), less_than_key(DT));
  }

  outs() << "Insiemi riordinati:\n";
  for (auto i = CFGEquivalents.begin(); i != CFGEquivalents.end(); ++i) {
    std::vector<Loop*> V = *i;

    for (auto j = V.begin(); j != V.end(); ++j) {
      Loop *L = *j;
      outs() << *L << "\n";
    }
  }

  for (auto i = CFGEquivalents.begin(); i != CFGEquivalents.end(); ++i) {
    std::vector<Loop*> V = *i;

    for (auto j = V.begin(); j != V.end(); ++j) {
      Loop* L1 = *j;

      for (auto k = ++j; k != V.end(); ++k) {
        Loop* L2 = *k;
        
        // Controllo n°1.
        if (L1->getExitBlock() == L2->getLoopPreheader()) {
          int instruction_count = 0;
          BasicBlock *MiddleBlock = L1->getExitBlock();

          for (auto iter_block = MiddleBlock->begin(); iter_block != MiddleBlock->end(); ++iter_block) {
            ++instruction_count;
          }

          if (instruction_count != 1) {
            // std::cout << "Loop non adiacenti!" << std::endl;
            continue;
          }

          /*
          Instruction &I = *MiddleBlock->begin();
          BranchInst *BI = dyn_cast<llvm::BranchInst*>(I);

          if (!(BI && BI->getSuccessor(0) == L2->getHeader())) {
            continue;
          }
          */
        }

        BasicBlock *L1L = L1->getLoopLatch();
        Instruction *I1 = dyn_cast<Instruction>(L1L->end());
        BranchInst *BI1 = dyn_cast<llvm::BranchInst>(I1);
        BI1->setSuccessor(0, L2->getHeader());
        
        BasicBlock *HB1 = L1->getHeader();
        Instruction *I2 = dyn_cast<Instruction>(HB1->end());
        BranchInst *BI2 = dyn_cast<llvm::BranchInst>(I2);
        BI2->setSuccessor(1, L2->getHeader());

        BasicBlock *H2L = L2->getLoopLatch();
        Instruction *I3 = dyn_cast<Instruction>(H2L->end());
        BranchInst *BI3 = dyn_cast<llvm::BranchInst>(I3);
        BI2->setSuccessor(0, L1->getHeader());

        std::cout << "Loop 1 latch:" << std::endl;
        outs() << *L1L << "\n";
      }
    }
  }



  PreservedAnalyses PA = PreservedAnalyses();
  return PA;
}