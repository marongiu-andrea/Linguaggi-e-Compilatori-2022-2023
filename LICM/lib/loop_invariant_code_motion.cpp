#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>

using namespace llvm;

// Deve esserci un "static int count = 0;" all'inizio della funzione
#define Debug_Print do { outs() << __FUNCTION__ << " " << count << "\n"; ++count; } while(0)
#define Debug_Print_Message(message) do { outs() << __FUNCTION__ << " " << count << " " << (message) << "\n"; ++count; } while(0)

namespace {
    
    class LoopInvariantCodeMotion final : public LoopPass {
        public:
        static char ID;
        
        LoopInvariantCodeMotion() : LoopPass(ID) {}
        
        virtual void getAnalysisUsage(AnalysisUsage &AU) const override {
            AU.addRequired<DominatorTreeWrapperPass>();
            AU.addRequired<LoopInfoWrapperPass>();
        }
        
#if 1
        struct DFSIterator {
            std::set<BasicBlock*> visited;
            std::vector<BasicBlock*> blockStack;
            
            DFSIterator(BasicBlock *block) { blockStack.push_back(block); };
            
            void reset(BasicBlock *block) {
                visited.clear();
                blockStack.clear();
                blockStack.push_back(block);
            };
            
            BasicBlock *next() {
                if(blockStack.empty())
                    return nullptr;
                
                auto res = blockStack[blockStack.size() - 1];
                blockStack.pop_back();
                visited.insert(res);
                
                for(auto succ : successors(res)) {
                    if(!visited.count(succ))
                        blockStack.push_back(succ);
                }
                
                return res;
            };
        };
        
        // Suppone che i nodi vengano visitati in DFS
        bool isLoopInvariant(Instruction *instr, Loop *loop) {
            for(auto it = instr->op_begin(); it != instr->op_end(); ++it) {
                auto def      = dyn_cast<Instruction>(*it);
                auto constOp  = dyn_cast<Constant>(*it);
                auto argument = dyn_cast<Argument>(*it);
                
                if(!def && !constOp && !argument)
                    return false;
                
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
        
        bool isSafeToMove(Instruction *instr, Loop *loop) {
            DominatorTree* dt = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
            
            SmallVector<BasicBlock*> vec;
            // Tutte le uscite del blocco
            loop->getExitBlocks(vec);
            for(auto it = vec.begin(); it != vec.end(); ++it)
            {
                BasicBlock* exitBlock = *it;
                if(!dt->dominates(instr, exitBlock))
                    return false;
            }
            
            return true;
        }
        
        virtual bool runOnLoop(Loop* l, LPPassManager& lpm) override {
            if(!l)
                return false;
            
            bool modified = false;
            
            DFSIterator it(l->getHeader());
            while(auto block = it.next()) {
                for(auto instr = block->begin(); instr != block->end(); ++instr) {
                    if(isLoopInvariant(&*instr, l) && isSafeToMove(&*instr, l)) {
                        LLVMContext &ctx = instr->getContext();
                        MDNode *node = MDNode::get(ctx, MDString::get(ctx, "hoistable"));
                        instr->setMetadata("HOIST", node);
                    }
                }
            }
            
            it.reset(l->getHeader());
            while(auto block = it.next()) {
                Instruction *instr = &block->front();
                Instruction *next = 0;
                while(instr) {
                    next = instr->getNextNode();
                    
                    if(instr->hasMetadata("HOIST")) {
                        assert(l->isLoopSimplifyForm());
                        
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
#else
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
#endif
    };
    
    char LoopInvariantCodeMotion::ID = 0;
    RegisterPass<LoopInvariantCodeMotion> X("loop-invariant-code-motion",
                                            "Loop Invariant Code Motion");
    
} // anonymous namespace

