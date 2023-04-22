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
    std::map <Value*, state> LIMap;    
    // Set dei basic block.    
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
    std::cout << "Map size: " << LIMap.size() << std::endl;
    // Debug.
    for (auto iter_map = ++LIMap.begin(); iter_map != LIMap.end(); ++iter_map) {
      outs() << iter_map->first << " " << *iter_map->first<<" --> "<<iter_map->second;
      std::cout << std::endl;
    }  
    return false; 
  }
};

char LoopInvariantCodeMotionPass::ID = 0;
RegisterPass<LoopInvariantCodeMotionPass> X("loop-walk", "Loop Walk");

} // anonymous namespace

