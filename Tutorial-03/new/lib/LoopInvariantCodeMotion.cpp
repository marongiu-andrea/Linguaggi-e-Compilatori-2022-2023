#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/SmallSet.h"

using namespace llvm;
#include <iostream>

#include <map>
#include <set>
#include <vector>

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
    // Enum per la mappa delle istruzioni loop-invariant.
    enum state : int
    {
      t = 2,
      f = 1
    };
    // Mappa delle istruzioni loop-invariant.
    std::map <Value*, state> LIMap;

    // Loop preheader.
    BasicBlock* PH = L->getLoopPreheader();
    // Debug preheader.
    if (!PH) {
      std::cout << "Il loop non è in forma canonica!" << std::endl;
      return false;
    }    
    outs() << *PH->getTerminator() << "\n";

    // Parte 1 - trovare le istruzioni loop-invariant.
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;
      for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
        Instruction& I = *iter_inst;
				bool invariant = true;
        if (I.isBinaryOp() && !(I.getOpcode() == 55)) {
					for (auto operand = I.op_begin(); operand != I.op_end(); ++operand) {
            Value *Operand = *operand;
						
						if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)){              
              continue;
            }
            else {              
              outs() << "------------------------\n";
              outs() <<"Analyzing: ";
              outs()<<I;
              std::cout<<std::endl;
              Instruction *Inst = dyn_cast<Instruction>(Operand);                                   
              outs() <<"Converting: ";
              outs()<<Operand<<*Operand<<" ---> "<<Inst<<"\n";
              if(!Inst){ // in this case, we assume that the reac.def. is the function argument              
                Argument *arg = dyn_cast<Argument>(Operand);
                if(arg){
                  outs()<<"Found function argument: ";
                  outs()<<*arg<<"\n";                  
                }
                else{
                  outs()<<"Error, unexpected type of operand\n";
                  invariant = false;
                }
                continue; // avoid lookup on null value;
              }
              // when converting %0, the cast does not work well, and gives
              // in output 0x0. Then when we lookup the map it obv. returns 1,
              // because 0x0 is not memorized. So the algo works anyway, but 
              // it might be dangerous.              
              if (LIMap[Inst] == f) {
                invariant = false;                
              }
              else {                
                continue;
              }
            }							
					}          
          if (invariant) {
            LIMap[&I] = t;            
          }
          else{
            LIMap[&I] = f;            
          }            
        }
        else {
          LIMap[&I] = f;          
        }    
      }
    }

    // Debug.
    std::cout << "Map size: " << LIMap.size() << std::endl;
    
    for (auto iter_map = LIMap.begin(); iter_map != LIMap.end(); ++iter_map) {
      outs() << iter_map->first << " " << *iter_map->first<<" --> "<<iter_map->second;
      std::cout << std::endl;
    }

    // Parte 2 - definire le istruzioni candidate alla code motion.
    std::set<BasicBlock*> ExitBlocks;
    std::set<Value*> CMCandidates;
    // llvm::SmallSet<Value*> CMCandidates;

    std::cout << "Uscite del loop" << std::endl; // CAN BE CLEANER
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      if (L->isLoopExiting(BB)) {
        ExitBlocks.insert(BB);
        // Debug.
        outs() << *BB << "\n";
      }
    }

    std::cout << "Istruzioni in blocchi che dominano le uscite del loop:" << std::endl;
    for (Loop::block_iterator BI = L->block_begin(); BI != L->block_end(); ++BI) {
      llvm::BasicBlock *BB = *BI;

      for (auto iter_inst = BB->begin(); iter_inst != BB->end(); ++iter_inst) {
        Instruction& I = *iter_inst;

        if (LIMap[&I] == 2) {

          for (auto iter_set = ExitBlocks.begin(); iter_set != ExitBlocks.end(); ++iter_set) {
            BasicBlock *EB = *iter_set;
            
            if (!DT->dominates(I.getParent(), EB)) {
              break;
            }

            if (iter_set == --ExitBlocks.end()) {
              bool movable = true;
              outs() << I << "\n";

              std::cout << "Usi dell'istruzione:" << std::endl;
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
    // std::vector<Instruction*> toAdd;
    llvm::SmallVector<Instruction*> toAdd;
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

