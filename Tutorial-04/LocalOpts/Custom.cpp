#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>

using namespace llvm;

#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/ScalarEvolution.h"


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
  auto &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

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
            continue;
          }

          Instruction *I = dyn_cast<Instruction>(MiddleBlock->begin());
          BranchInst *BI = dyn_cast<llvm::BranchInst>(I);

          if (!(BI && BI->getSuccessor(0) == L2->getHeader())) {
            continue;
          }

          // Controllo n°2.
          if (!((SE.getSmallConstantTripCount(L1) == SE.getSmallConstantTripCount(L2)) &&
            (SE.getSmallConstantMaxTripCount(L1) == SE.getSmallConstantMaxTripCount(L2))))
            continue;
          
          // Controllo n°3 già garantito dalle operazioni preliminari.
        }

        
        outs()<<"-----------Performing Loop Fusion------------------\n";                

        // PART 1--------------BODY LOOP1 --> BODY LOOP2----------------     
        BasicBlock *FINAL = L2->getExitBlock();
        outs()<<"ExitBlock di Loop2 "<<*L2->getExitBlock()<<"\n";

        BasicBlock *LL1 = L1->getLoopLatch();                 // Latch loop1.
        outs()<<"Latch loop1: "<<*LL1<<"\n";

        BasicBlock *LB1 = LL1->getPrevNode();                 // Body loop1.        
        outs()<<"Body loop1: "<<*LB1<<"\n";        

        BasicBlock *HB2 = L2->getHeader();                    // Header loop2.
        outs()<<"Header loop2: "<<*HB2<<"\n";        
        Instruction* L2PHI = dyn_cast<Instruction>(HB2->begin());
        outs()<<"FINAL DEBUG: PHINODE of Loop2 "<<L2PHI<<"\n";
        
        Instruction *I1 = HB2->getTerminator();        
        BranchInst *BI1 = dyn_cast<llvm::BranchInst>(I1);        
        outs()<<"Terminatore come branch instruction"<<*BI1<<"\n";

        BasicBlock *LB2 = BI1->getSuccessor(0);               
        outs()<<"Successore T del branch dell'header di loop2, quindi primo blocco del body:"<<*LB2<<"\n";


        Instruction *I2 = LB1->getTerminator();
        BranchInst *BI2 = dyn_cast<llvm::BranchInst>(I2);
        outs()<<"Terminatore come branch inst di Loop1 "<<*BI2<<"\n";        
        BI2->setSuccessor(0, LB2);
        outs()<<"Successore modificato del primo loop: "<<*BI2->getSuccessor(0)<<"\n";
        // PART 2-------------BodyLoop2 -> LatchLoop1----------------------------

        BasicBlock *LL2 = L2->getLoopLatch();                 // Latch loop2.
        outs() << "Latch loop 2: "<<*LL2<<"\n";
        
        BasicBlock *LB2E = LL2->getPrevNode();
        Instruction *LB2E_branch = LB2E->getTerminator();
        outs()<<"TERMINATOR: "<<*LB2E_branch<<"\n";

        LB2E_branch->setSuccessor(0,LL1);


        // Instruction *I3 = LL2->getTerminator();
        // BranchInst *BI3 = dyn_cast<llvm::BranchInst>(I3);
        // outs()<<"Branch del latch di loop2: "<<*BI3<<"\n";
        
        BasicBlock *HB1 = L1->getHeader();                    // Header loop1.
        Instruction* L1PHI = dyn_cast<Instruction>(HB1->begin());
        outs()<<"FINAL DEBUG: "<<L1PHI<<"\n";

        // BI3->setSuccessor(0, HB1);                            // Latch loop2 -> header loop1.
        // outs()<<"Successore del latch di Loop2 dopo l'aggancio: "<<*BI3->getSuccessor(0)<<"\n";

        // PART 3-----------Header Loop1->ExitLoop2--------------------------------

        Instruction *I4 = HB1->getTerminator();// Branch header loop1.
        BranchInst *BI4 = dyn_cast<llvm::BranchInst>(I4);     //
        outs()<<"Branch dell'header di Loop1 come branch: "<<*BI4<<"\n";
                        
        BI4->setSuccessor(1, FINAL);                            // Header loop1 -> exit loop2.
        outs() << "Successore dell'header di Loop1 dopo l'aggancio: "<<*BI4->getSuccessor(1)<<"\n";        
        Value *CastedL1PHI = dyn_cast<Value>(L1PHI);		                
        outs()<<"L2PHI: "<<*L2PHI<<"\n";        

        // L2PHI->replaceAllUsesWith(CastedL1PHI);
        // L2PHI->eraseFromParent();

        std::vector<std::pair<Instruction*, int>> temp;

        for (auto iter_use = L2PHI->user_begin(); iter_use != L2PHI->user_end(); ++iter_use) {
          Instruction *U = dyn_cast<Instruction>(*iter_use);
          outs() <<"User: "<<*U<<"\n";
          BasicBlock *block = U->getParent();
          if(L2->contains(block)){           
            int f = 0;            
            for (auto operand = U->op_begin(); operand != U->op_end(); ++operand) {
              Value *Operand = *operand;
              if (dyn_cast<Constant>(Operand)){
                f++;
                outs()<<"Costant\n";
                continue;
              }
              Instruction* castedOperand = dyn_cast<Instruction>(Operand);
              outs() << "Casted operand: "<< *castedOperand << "\n";
              outs() <<"Index: "<<f<<"\n";
              if(castedOperand == L2PHI){
                std::pair<Instruction*, int> to_add = std::make_pair(U, f);                
                temp.push_back(to_add);
              }
              f++;
            }
          }
        }  
        for(auto iter_vector = temp.begin(); iter_vector != temp.end(); iter_vector++){          
          Instruction *test = (iter_vector->first);
          if(test->getParent() != LL2 && test->getParent() != HB2){
            outs()<<*(iter_vector->first)<<" --> "<<(iter_vector->second)<<"\n";
            test->setOperand((iter_vector->second), CastedL1PHI);
            outs()<<*test<<"\n";
          }
        }
        // I1->setSuccessor(0,LL2);
      }
    }
  }
  PreservedAnalyses PA = PreservedAnalyses();
  return PA;
}