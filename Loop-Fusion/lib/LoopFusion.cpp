
#include "LoopFusion.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/ADT/SmallSet.h>

#include <cmath>
#include <stdint.h>

using namespace llvm;

bool LoopFusionPass::isLoopAdjacent(llvm::Loop *a, llvm::Loop *b) 
{
    if (!a || !b)
        return false;

    auto *eb = a->getExitBlock();
    if (!eb)
        return false;

    SmallVector<BasicBlock *, 4> exits;
    a->getExitBlocks(exits);

    auto *preheader = b->getLoopPreheader();
    
    auto contains = false;
    for (auto *bb: exits) {
        if (bb == preheader) {
            contains = true;
            break;
        }
    }

    if (!contains)
        false;

    if (preheader->size() > 1)
        return false;

    Instruction *instr = &preheader->front();
    BranchInst *br = dyn_cast<BranchInst>(instr);
    if (!br || br->getNumSuccessors() != 1)
        return false;

    return (br->getSuccessor(0) == b->getHeader());
}

bool LoopFusionPass::checkBounds(llvm::Loop *a, llvm::Loop *b, ScalarEvolution &sce)
{
    auto foptBounds = a->getBounds(sce);
    auto soptBounds = b->getBounds(sce);
    
    if (!foptBounds.has_value() || !soptBounds.has_value()) {
        outs() << "no boundings \n";
        return false;
    }

    outs() << "boundings found\n";

    auto fBounds = foptBounds.value();
    auto sBounds = soptBounds.value();

    //fBounds.getInitialIVValue().dump();
    //fBounds.getFinalIVValue().dump();
    //fBounds.getStepValue()->dump();
//
    //sBounds.getInitialIVValue().dump();
    //sBounds.getFinalIVValue().dump();
    //sBounds.getStepValue()->dump();
//
    //outs() << " count " << sce.getSmallConstantTripCount(a) << "\n";
    //outs() << " count " << sce.getSmallConstantTripCount(b) << "\n";

    auto fGuard = a->getLoopGuardBranch();
    auto sGuard = b->getLoopGuardBranch();

    if ((!fGuard && sGuard) || (fGuard && !sGuard))
        return false;

    if (fGuard && sGuard) {
        auto cond1 = dyn_cast<llvm::User>(fGuard->getCondition());
        auto cond2 = dyn_cast<llvm::User>(sGuard->getCondition());

        if (cond1 != cond2){
            if (cond1->getType() != cond2->getType())
                return false;

            outs() << "branch condition type ok\n"; 
            if (cond1->getNumUses() != cond2->getNumUses())
                return false;

            
            outs() << "branch uses equals ok\n"; 

            SmallSet<Value *, 2> values;
            for (auto &op: cond1->operands())
                values.insert(op);

            SmallSet<Value *, 2> values2;
            for (auto &op: cond2->operands())
                values2.insert(op);

            for (auto *v : values)
                if (!values2.contains(v)) {
                    outs() << "operands does not match \n";
                    return false;
                }
            
            outs() << "same operands for guard\n";
        } 
    }

    return (&fBounds.getInitialIVValue() == &sBounds.getInitialIVValue() &&
        &fBounds.getFinalIVValue() == &sBounds.getFinalIVValue() &&
        fBounds.getStepValue() == sBounds.getStepValue() &&
        sce.getSmallConstantTripCount(a) == sce.getSmallConstantTripCount(b)
    );
}

bool LoopFusionPass::checkDominance(llvm::Loop *a, llvm::Loop *b, llvm::DominatorTree &dt, 
                        llvm::PostDominatorTree &pdt) 
{
    SmallVector<BasicBlock *, 2> exits;
    a->getExitBlocks(exits);

    auto dominates = true;
    auto *preheader = b->getLoopPreheader();
    preheader->dump();
    for (auto exit: exits) {
        dominates |= dt.dominates(exit, preheader);
        dominates |= pdt.dominates(preheader, exit);
    }
    
    return dominates;
}

void LoopFusionPass::loopFusion(llvm::Loop *a, llvm::Loop *b, llvm::ScalarEvolution &sce) 
{
    auto *header1 = a->getHeader();
    auto *header2 = b->getHeader();

    auto *latch1 = a->getLoopLatch();
    auto *latch2 = b->getLoopLatch();

    auto *preheader1 = a->getLoopPreheader();
    auto *preheader2 = b->getLoopPreheader();
    
    auto *indvar1  = a->getInductionVariable(sce);
    auto *indvar2  = b->getInductionVariable(sce);
    
    auto *exitBlock1 = a->getExitBlock();
    auto *exitBlock2 = b->getExitBlock();
    
    auto *aLoopG = a->getLoopGuardBranch();
    auto *bLoopG = b->getLoopGuardBranch();

    // moving induction var to header1 
    indvar2->moveBefore(indvar1);
    indvar1->replaceAllUsesWith(indvar2);
    indvar1->eraseFromParent();

    // update phi instruction 
    indvar2->replaceIncomingBlockWith(preheader2, preheader1);

    // update edges to header2 to header1
    header2->replaceAllUsesWith(header1);

    // make header2 the successor of header1
    auto * instr = header1->getTerminator();
    auto *br = dyn_cast<BranchInst>(instr);
    br->setSuccessor(0, header2);
    
    if (bLoopG) {
        auto *gb = bLoopG->getParent();
        auto *ga = aLoopG->getParent();

        gb->replaceAllUsesWith(exitBlock2->getSingleSuccessor());
        exitBlock1->eraseFromParent();

        gb->eraseFromParent();
    }

    //removing latch1 and preheader 2
    latch1->replaceAllUsesWith(latch2);
    preheader2->eraseFromParent();
    latch1->eraseFromParent();
}

// warning: apply first this optimizations 
// mem2reg
// indvars
// loop-simplify
// loop-rotate
llvm::PreservedAnalyses LoopFusionPass::run(llvm::Function &f,
                                            llvm::FunctionAnalysisManager &FM) 
{
    auto &loopinfo = FM.getResult<LoopAnalysis>(f); 
    auto &sce = FM.getResult<ScalarEvolutionAnalysis>(f);
    auto &dt = FM.getResult<DominatorTreeAnalysis>(f);
    auto &pdt = FM.getResult<PostDominatorTreeAnalysis>(f);

    auto loops = loopinfo.getLoopsInPreorder();

    //outs() << "started\n";
    for (int i = 0; i < loops.size() - 1; i++) {
        if (!isLoopAdjacent(loops[i], loops[i + 1]))
            continue;

        outs() << "loop adjacent ok \n";

        if (!checkBounds(loops[i], loops[i + 1], sce))
            continue;
        
        
        outs() << "loop bouidings ok \n";

        if (!checkDominance(loops[i], loops[i+1], dt, pdt))
            continue;

        
        outs() << "loop dominance ok \n";

        //outs() << "loop fusion executings";

        loopFusion(loops[i], loops[i + 1], sce);

        outs() << " dumping function \n\n";
        f.dump();
        break;
    }

    return PreservedAnalyses::none();
}