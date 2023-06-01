#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/SmallVector.h"

using namespace llvm;
#include <iostream>

#include <map>
#include <set>
#include <vector>


namespace {

class LoopInvariantCodeMotionPass final : public LoopPass {
public:
  static char ID;

  // Enum per la mappa delle istruzioni loop-invariant.
  enum state : int
  {
    t = 2,
    f = 1
  };

  LoopInvariantCodeMotionPass() : LoopPass(ID) {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
  }

  state isLoopInvariant(Instruction &I, std::map <Value*, state> &LIMap) {
    bool invariant = true;
    if (I.isBinaryOp() && !(I.getOpcode() == 55)) {
	  // we focus on binary operations, and skip PHI nodes, that
	  // are for sure loop invariant.
      for (auto operand = I.op_begin(); operand != I.op_end(); ++operand) {
        Value *Operand = *operand;
        
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)){
          continue;
        }
        else {
          outs() << "------------------------\n";
          outs() << "Analyzing: " << I << "\n";

          Instruction *Inst = dyn_cast<Instruction>(Operand);

          outs() <<"Converting: ";
          outs() << Operand << *Operand << " ---> " << Inst << "\n";
          
          // Debug operands
          if(!Inst){
            // In this case, we assume that the reaching definition is
            // the function argument.
            Argument *arg = dyn_cast<Argument>(Operand);
            if(arg){
              outs() << "Found function argument: ";
              outs() << *arg << "\n"; 
            }
            else{
              outs() << "Error, unexpected type of operand\n";
			  invariant = false;			  
            }
            continue; // Avoid lookup on null value;
          }
          if (LIMap[Inst] == f) invariant = false;
          else continue;
        }
      }      
      if (invariant) return t;
      else return f;
    }
    else return f;
  }

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM) override {
    outs() << "\nLOOP-INVARIANT CODE MOTION INIZIATO...\n";
    DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
    // Map to store loop-invariant Instructions, to avoid long chains of function calls.
      std::map <Value*, state> LIMap;
      // Loop preheader.
    if(!L->isLoopSimplifyForm()){
        std::cout << "Loop is not in canonical form!" << std::endl;
        return false;	  
    }	
      BasicBlock* PH = L->getLoopPreheader();
    
      // Part 1 - find loop invariant instructions
      std::cout << "########## Loop-Invariant check debug ##########" << std::endl;
      for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
        llvm::BasicBlock *BB = *BI;

        for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
          Instruction& I = *iter_inst;
          LIMap[&I] = isLoopInvariant(I, LIMap);
        }
      }

      std::cout << "########## Map debug ##########" << std::endl;
      std::cout << "Map size: " << LIMap.size() << std::endl;
      for (auto iter_map = LIMap.begin(); iter_map != LIMap.end(); ++iter_map) {
        outs() << iter_map->first << " " << *iter_map->first << " --> " << iter_map->second << "\n";
      }


      // Part 2 - find code motion candidates.
      
      // set of BB's that exit from loop.
      // std::set<BasicBlock*> ExitBlocks;
      // Set of code motion candidates.
      std::set<Value*> CMCandidates;
      // llvm::SmallSet<Value*> CMCandidates;

      std::cout << "Loop exiting blocks:" << std::endl;
    llvm::SmallVector<llvm::BasicBlock*> ExitingBlocks;
    L->getExitingBlocks(ExitingBlocks);
    // for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
    //   llvm::BasicBlock *BB = *BI;

    //   if (L->isLoopExiting(BB)) {
    //     ExitBlocks.insert(BB);
    //     // Debug basic block in uscita dal loop.
    //     outs() << *BB << "\n";
    //   }
    // }

    std::cout << "Instructions in blocks that dominate loop exits:" << std::endl;
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
        Instruction& I = *iter_inst;

        if (LIMap[&I] == 2) {

          for (int i = 0; i < ExitingBlocks.size(); i++) {
            BasicBlock *EB = ExitingBlocks[i];
            if (!DT->dominates(I.getParent(), EB)) {
              break;			
            }
            if (i == ExitingBlocks.size()-1) { // if it dominates all exiting blocks,
			  // then we can check the last condition
              bool movable = true;
              outs() << I << "\n";

              std::cout << "Instruction's uses:" << std::endl;
              for (auto iter_use = I.user_begin(); iter_use != I.user_end(); ++iter_use) {
                Instruction *U = dyn_cast<Instruction>(*iter_use);

                if (L->contains(U)) {
                  if (DT->dominates(I.getParent(), U->getParent())) {
                    continue;
                  }
                  else {
                    movable = false;
                  }
                }
              }
              if (movable) {
                outs() << I << "\n";
                CMCandidates.insert(&I);
              }

              outs() << "------------------------\n";
            }
          }
        }
      }
    }
    llvm::SmallVector<Instruction*> toAdd; // using small vector to improve performance
    for (
      auto node = GraphTraits<DominatorTree *>::nodes_begin(DT);
      node != GraphTraits<DominatorTree *>::nodes_end(DT);
      ++node
      ) {
        BasicBlock *BB = node->getBlock();
        outs() << *BB << "\n";

        for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
          Instruction& I = *iter_inst;

          if (CMCandidates.find(&I) != CMCandidates.end()) {
            toAdd.push_back(&I);
          }
        }
    }	
    for (auto iter_vector = toAdd.begin(); iter_vector != toAdd.end(); ++iter_vector) {
      Instruction *I = *iter_vector;
      I->removeFromParent();
      I->insertBefore(PH->getTerminator());
    }

    for (
      auto node = GraphTraits<DominatorTree *>::nodes_begin(DT);
      node != GraphTraits<DominatorTree *>::nodes_end(DT);
      ++node
      ) {
        BasicBlock *BB = node->getBlock();
        outs() << *BB << "\n";
    }
    
    return false;
  }
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

