#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Use.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/IRBuilder.h>
using namespace llvm;

class LoopWalkPass final : public LoopPass
{
  public:
    static char ID;

    LoopWalkPass() :
        LoopPass(ID) {}

    void getAnalysisUsage(AnalysisUsage& AU) const override
    {
        AU.addRequired<DominatorTreeWrapperPass>();
        AU.addRequired<LoopInfoWrapperPass>();
    }

    bool runOnLoop(Loop* L, LPPassManager& LPM) override
    {
        std::set<Instruction*> liiSet;
        std::set<BasicBlock*> exitingBB;
        std::set<Instruction*> hoistingSet;
        DominatorTree& dt = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
        getLoopInvariantInstructionsAndExitingBlocks(L, liiSet, exitingBB);
        getHoistableInstructions(L, dt, liiSet, exitingBB, hoistingSet);
        BasicBlock* preheader = L->getLoopPreheader();
        outs() << "Loop invariant:\n";
        for(Instruction* i: liiSet)
        {
            outs() << *i << '\n';
        }
        outs() << "hoistable:\n";
        for(Instruction* i: hoistingSet)
        {
            outs() << *i << '\n';
        }
        if(preheader)
        {
            IRBuilder<> builder(preheader, preheader->begin());
            for (Instruction *i: hoistingSet)
            {
                i->removeFromParent();
                builder.Insert(i);
            }
        }
        else
        {
            outs() << "Loop senza preheader";
            return false;
        }
        return true;
    }
private:
    void getLoopInvariantInstructionsAndExitingBlocks(Loop* L, std::set<Instruction*>& liInstrs, std::set<BasicBlock*>& exitingBlocks)
    {
        for(BasicBlock* bb: L->getBlocksVector())
        {
            for(auto& i: *bb)
            {
                if(i.isBinaryOp() && isInstructionLoopInvariant(L, liInstrs, i))
                {
                    liInstrs.insert(&i);
                }
            }
            if(L->isLoopExiting(bb))
                exitingBlocks.insert(bb);
        }
    }
    
    bool isInstructionLoopInvariant(Loop* L, std::set<Instruction*> const& liInstrs, Instruction const& i)
    {
        bool loop_inv = true;
        for(const Use& u: i.operands())
        {
            Value *v = u.get();
            if(Constant *C = dyn_cast<Constant>(v))
                continue;
            else if (Argument *A = dyn_cast<Argument>(v))
                continue;
            else if(Instruction *I = dyn_cast<Instruction>(v))
            {
                if(L->contains(I) && !liInstrs.contains(I))
                {
                    loop_inv = false;
                    break;
                }
            }
        }
        return loop_inv;
    }
    
    void getHoistableInstructions(Loop* L, DominatorTree const& DT, std::set<Instruction*> const& liInstrs, std::set<BasicBlock*> const& exitingBlocks, std::set<Instruction*>& hoistableInstrs)
    {
        for(auto* inst: liInstrs)
        {
            bool dominates = true;
            for(auto it = exitingBlocks.begin(); dominates && it != exitingBlocks.end(); it++)
            {
                dominates = DT.dominates(inst, *it);
            }
            if(dominates)
                hoistableInstrs.insert(inst);
            else
            {
                bool hoistable = true;
                for(Use& use: inst->uses())
                {
                    Instruction* i = dyn_cast<Instruction>(use.getUser());
                    if(!i)
                        outs() << "Attenzione: questo user non è un'istruzione:" << use.getUser() << '\n';
                    if(i && !L->contains(i))
                    {
                        hoistable = false;
                        break;
                    }
                }
                if(hoistable)
                    hoistableInstrs.insert(inst);
            }
        }
    }
};

char LoopWalkPass::ID = 0;
RegisterPass<LoopWalkPass> X("custom-licm", "Custom LICM Pass");
