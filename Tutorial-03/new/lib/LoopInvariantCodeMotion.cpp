#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/LoopInfo.h"

using namespace llvm;
#include <iostream>

#include <map>
#include <set>

namespace {

class LoopInvariantCodeMotionPass final : public LoopPass {
public:
  static char ID;

  LoopInvariantCodeMotionPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOP-INVARIANT CODE MOTION INIZIATO...\n";

    // Dominance tree.
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    // Enum.
    enum state : int
    {
      t = 2,
      f = 1
    };
    // Mappa delle istruzioni.
    std::map <Instruction*, state> LIMap;
    std::cout << "Map size: " << LIMap.size() << std::endl;

    // Set dei basic block.
    std::set <BasicBlock*> BBSet;

    // Parte 1 - trovare le istruzioni loop-invariant.
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
        Instruction& I = *iter_inst;
				bool invariant = true;

        outs() << I;
        std::cout << std::endl;

        if (I.isBinaryOp() && !(I.getOpcode() == 55)) {
					for (auto operand = I.op_begin(); operand != I.op_end(); ++operand) {
            Value *Operand = *operand;
						
						if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)){
              std::cout << "Constant!" << std::endl;
              continue;
            }
            else {
              Instruction *Inst = dyn_cast<Instruction>(Operand);
              std::cout << LIMap.size() << " " << LIMap[&I] << std::endl;
              if (LIMap[Inst] == f) {
                invariant = false;
                std::cout << "False!" << std::endl;
              }
              else {
                std::cout << "True or NULL!" << std::endl;
                continue;
              }
            }
							
					}

          outs() << I;

          if (invariant) {
            LIMap[&I] = t;
            std::cout << " Loop-invariant!" << std::endl;
          }
          else{
            LIMap[&I] = f;
            std::cout << " NON loop-invariant!" << std::endl;
          }
            
        }
        else {
          LIMap[&I] = f;
          outs() << I;
          std::cout << " NON loop-invariant!" << std::endl;
        }

        std::cout << LIMap.size() << " " << LIMap[&I] << std::endl;
      }
    }

    std::cout << "Map size: " << LIMap.size() << std::endl;


    // Debug.
    for (auto iter_map = ++LIMap.begin(); iter_map != LIMap.end(); ++iter_map) {
      std::cout << iter_map->first << " " << iter_map->second << std::endl;
      //outs() << *(iter_map->first);
      std::cout << std::endl;
    }

    

		

    return false; 
  }
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

