
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

#include <unordered_map>

using namespace llvm;

// Deve esserci un "static int count = 0;" all'inizio della funzione
#define Debug_Print do { outs() << __FUNCTION__ << " " << count << "\n"; ++count; } while(0)
#define Debug_Print_Message(message) do { outs() << __FUNCTION__ << " " << count << " " << (message) << "\n"; ++count; } while(0)

static const char* functionNameToDebug = "gb_alloc_str";

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

namespace {
    
    class LoopInvariantCodeMotion final : public LoopPass {
        public:
        static char ID;
        
        LoopInvariantCodeMotion() : LoopPass(ID) {}
        
        virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<DominatorTreeWrapperPass>();
            AU.addRequired<LoopInfoWrapperPass>();
        }
        
        struct DFSIterator {
            std::set<BasicBlock*> visited;
            std::vector<BasicBlock*> blockStack;
            Loop* loop = 0;
            
            DFSIterator(Loop* loop) { this->loop = loop; blockStack.push_back(loop->getHeader()); };
            
            void reset(Loop* loop) {
                this->loop = loop;
                visited.clear();
                blockStack.clear();
                blockStack.push_back(loop->getHeader());
            };
            
            BasicBlock *next() {
                if(blockStack.empty())
                    return nullptr;
                
                auto res = blockStack[blockStack.size() - 1];
                blockStack.pop_back();
                visited.insert(res);
                
                for(auto succ : successors(res)) {
                    if(!visited.count(succ) && loop->contains(succ))
                        blockStack.push_back(succ);
                }
                
                return res;
            };
        };
        
        bool instrHasSideEffects(Instruction* instr) {
            if(dyn_cast<ReturnInst>(instr)) return true;
            if(dyn_cast<BranchInst>(instr)) return true;
            if(dyn_cast<PHINode>(instr))    return true;
            if(dyn_cast<CallInst>(instr))   return true;
            if(dyn_cast<LoadInst>(instr))   return true;
            if(dyn_cast<StoreInst>(instr))  return true;
            
            return false;
        }
        
        struct TableEntry
        {
            Instruction* key;
            char value[0];  // Dimensione nulla
        };
        
        // Suppone che i nodi vengano visitati in DFS
        bool isLoopInvariantHashTable(Instruction *instr, Loop *loop, TableEntry* table) {
            if(instrHasSideEffects(instr)) return false;
            
            for(auto it = instr->op_begin(); it != instr->op_end(); ++it) {
                auto def      = dyn_cast<Instruction>(*it);
                auto constOp  = dyn_cast<Constant>(*it);
                auto argument = dyn_cast<Argument>(*it);
                
                if(!def && !constOp && !argument)   return false;
                if(def && instrHasSideEffects(def)) return false;
                
                // 1. Tutte le definizioni che raggiungono l'istruzione
                // si trovano fuori dal loop (o sono costanti)
                // 2. C'è esattamente una reaching definition, e si tratta
                // di un'istruzione loop-invariant
                if(!argument && !constOp && loop->contains(def) && hmgeti(table, instr) == -1)
                    return false;
            }
            
            // Aggiungi nodo all'istruzione
            TableEntry newEntry = { instr };
            hmputs(table, newEntry);
            return true;
        }
        
        bool isLoopInvariantRecursive(Value *instr, Loop *loop) {
            auto def = dyn_cast<Instruction>(instr);
            auto constOp = dyn_cast<Constant>(instr);
            auto argument = dyn_cast<Argument>(instr);
            
            if(constOp)                         return true;
            if(def && instrHasSideEffects(def)) return false;
            if(!def && !constOp && !argument)   return false;
            
            bool result = true;
            if(def) {
                if(!loop->contains(def))
                    result = true;
                else for(auto op = def->op_begin(); op != def->op_end() && result; ++op)
                    result &= isLoopInvariantRecursive((Value*)*op, loop);
            }
            
            return result;
        }
        
        // Suppone che i nodi vengano visitati in DFS
        bool isLoopInvariant(Instruction *instr, Loop *loop) {
            if(dyn_cast<ReturnInst>(instr)) return false;
            if(dyn_cast<BranchInst>(instr)) return false;
            if(dyn_cast<PHINode>(instr))    return false;
            if(dyn_cast<CallInst>(instr))   return false;
            if(dyn_cast<LoadInst>(instr))   return false;
            if(dyn_cast<StoreInst>(instr))  return false;
            
            for(auto it = instr->op_begin(); it != instr->op_end(); ++it) {
                auto def      = dyn_cast<Instruction>(*it);
                auto constOp  = dyn_cast<Constant>(*it);
                auto argument = dyn_cast<Argument>(*it);
                
                if(!def && !constOp && !argument) return false;
                if(dyn_cast<PHINode>(*it))        return false;
                if(dyn_cast<LoadInst>(*it))       return false;
                if(dyn_cast<StoreInst>(*it))      return false;
                if(dyn_cast<CallInst>(*it))       return false;
                
                // 1. Tutte le definizioni che raggiungono l'istruzione
                // si trovano fuori dal loop (o sono costanti)
                // 2. C'è esattamente una reaching definition, e si tratta
                // di un'istruzione loop-invariant
                if(!argument && !constOp && loop->contains(def) && !def->hasMetadata("LoopInvariant"))
                    return false;
            }
            
            // Aggiungi nodo all'istruzione
            LLVMContext& ctx = instr->getContext();
            MDNode *node = MDNode::get(ctx, MDString::get(ctx, "loopinvariant"));
            instr->setMetadata("LoopInvariant", node);
            
            return true;
        }
        
        bool isDead(Instruction *instr, Loop *loop) {
            bool result = true;
            
            for(auto user = instr->use_begin(); user != instr->use_end() && result; ++user) {
                Instruction *instr = dyn_cast<Instruction>(&*user);
                if(!loop->contains(instr))
                    result = false;
            }
            
            return result;
        }
        
        virtual bool runOnLoop(Loop* l, LPPassManager& lpm) override {
            if(!l)
                return false;
            
            // NOTE: Alcuni loop non sono in simplifyform nemmeno dopo il pass -loop-simplify
            //assert(l->isLoopSimplifyForm());
            if(!l->isLoopSimplifyForm())
                return false;
            
            DominatorTree *dt = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
            
#ifdef HashTableImpl
            TableEntry* hash = 0;
#endif
            
            // Analisi su quali istruzioni possono essere spostate
            
#ifndef RecursiveImpl
            DFSIterator it(l);
            while(BasicBlock* block = it.next()) {
#else  // Per l'implementazione ricorsiva non è necessaria una DFS per questa prima fase
                for(auto it = l->block_begin(); it != l->block_end(); ++it) {
                    BasicBlock* block = *it;
#endif
                    
                    bool dominatesAllExits = true;
                    
                    SmallVector<BasicBlock*> vec;
                    l->getExitBlocks(vec);
                    for(auto it = vec.begin(); it != vec.end(); ++it) {
                        BasicBlock *exitBlock = *it;
                        if(!dt->dominates(&*block, exitBlock))
                            dominatesAllExits = false;
                    }
                    
                    int numHoists = 0;
                    int numInstr = 0;
                    
                    for(auto instr = block->begin(); instr != block->end(); ++instr) {
#if defined HashTableImpl
                        bool isInv = isLoopInvariantHashTable(&*instr, l, hash);
#elif defined RecursiveImpl
                        bool isInv = isLoopInvariantRecursive(&*instr, l);
#elif defined MetadataImpl
                        bool isInv = isLoopInvariant(&*instr, l);
#endif
                        if(l->getHeader()->getParent()->getName().compare(StringRef(functionNameToDebug)) == 0)
                            if(isInv)
                            outs() << "inv: " << *instr << "\n";
                        
                        if(isInv && (dominatesAllExits || isDead(&*instr, l))) {
                            LLVMContext &ctx = instr->getContext();
                            MDNode *node = MDNode::get(ctx, MDString::get(ctx, "hoistable"));
                            instr->setMetadata("HOIST", node);
                            
                            ++numHoists;
                        }
                        
                        ++numInstr;
                    }
                }
                
                // Spostamento delle istruzioni
                
                bool modified = false;
                DFSIterator it2(l);
                while(BasicBlock* block = it2.next()) {
                    Instruction* instr = &block->front();
                    Instruction* next = 0;
                    while(instr) {
                        next = instr->getNextNode();
                        
                        if(instr->hasMetadata("HOIST")) {
                            // NOTE: Alcuni loop non sono in simplifyform nemmeno dopo il pass -loop-simplify
                            //assert(l->isLoopSimplifyForm());
                            
                            if(l->isLoopSimplifyForm()) {
                                modified = true;
                                instr->moveBefore(&l->getLoopPreheader()->back());
                            }
                        }
                        
                        instr = next;
                    }
                }
                
                return modified;
            }
        };
        
        char LoopInvariantCodeMotion::ID = 0;
        RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                                                "Loop Invariant Code Motion");
        
    } // anonymous namespace
    
    