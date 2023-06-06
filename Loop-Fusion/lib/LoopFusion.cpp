
#include "LoopFusion.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolution.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/Transforms/Utils/LoopSimplify.h>
#include <llvm/Transforms/Scalar/LoopRotation.h>
#include <llvm/ADT/Optional.h>

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
        return false;
    
    if (preheader->size() > 1)
        return false;
    
    Instruction *instr = &preheader->front();
    BranchInst *br = dyn_cast<BranchInst>(instr);
    if (!br || br->getNumSuccessors() != 1)
        return false;
    
    return (br->getSuccessor(0) == b->getHeader());
}

// NOTE(Leo): Le versioni successive a LLVM14 usano std::optional anzichè llvm::Optional
#ifdef LLVMOPT
template<typename t>
bool optionalHasValue(llvm::Optional<t>& optional) { return optional.hasValue(); }

template<typename t>
t optionalGetValue(llvm::Optional<t>& optional) { return optional.getValue(); }
#else
template<typename t>
bool optionalHasValue(std::optional<t>& optional) { return optional.has_value(); }

template<typename t>
t optionalGetValue(std::optional<t>& optional) { return optional.value(); }
#endif

bool LoopFusionPass::checkBounds(llvm::Loop *a, llvm::Loop *b, ScalarEvolution &sce)
{
    auto foptBounds = a->getBounds(sce);
    auto soptBounds = b->getBounds(sce);
    
    if (!optionalHasValue(foptBounds) || !optionalHasValue(soptBounds)) {
        outs() << "no bounds \n";
        return false;
    }
    
    outs() << "bounds found\n";
    
    auto fBounds = optionalGetValue(foptBounds);
    auto sBounds = optionalGetValue(soptBounds);
    
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
        dominates &= dt.dominates(exit, preheader);
        dominates &= pdt.dominates(preheader, exit);
    }
    
    return dominates;
}

void LoopFusionPass::loopFusion(llvm::Loop *a, llvm::Loop *b, llvm::ScalarEvolution &sce) 
{
    auto *header1 = a->getHeader();
    auto *header2 = b->getHeader();
    
    auto &blocks1 = a->getBlocksVector();
    auto &blocks2 = a->getBlocksVector();
    
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
    
    auto *successor = header1;
    while (successor != nullptr && successor->getSingleSuccessor() != latch1) {
        successor = header1->getSingleSuccessor();
    } 
    
    // make header2 the successor of header1
    auto * instr = successor->getTerminator();
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
    bool changed = true;
    while (changed) {
        changed = false;
        
        auto simplify = LoopSimplifyPass();
        FM.invalidate(f, simplify.run(f, FM));
        
        //auto rotate = LoopRotatePass();
        //FM.invalidate(f, rotate.run(f, FM));
        
        
        auto &loopinfo = FM.getResult<LoopAnalysis>(f); 
        auto &sce = FM.getResult<ScalarEvolutionAnalysis>(f);
        auto &dt = FM.getResult<DominatorTreeAnalysis>(f);
        auto &pdt = FM.getResult<PostDominatorTreeAnalysis>(f);
        
        auto loops = loopinfo.getLoopsInPreorder();
        
        for (int i = 0; i < loops.size() - 1; i++) {
            if (loops[i]->isGuarded() != loops[i+1]->isGuarded())
                continue;

            if (!loops[i]->isGuarded()) {
                if (!isLoopAdjacent(loops[i], loops[i + 1]))
                    continue;
            
                outs() << "loop adjacent ok \n";
            
                if (!checkDominance(loops[i], loops[i+1], dt, pdt))
                    continue;

                    
                outs() << "loop dominance ok \n";
            }

            if (!checkBounds(loops[i], loops[i + 1], sce))
                continue;
            
            outs() << "loop bounds ok \n";
            
            //outs() << "loop fusion executings";
            
            loopFusion(loops[i], loops[i + 1], sce);
            
            outs() << "dumping function \n\n";
            
            FM.invalidate(f, PreservedAnalyses::none());
            f.dump();
            changed = true;
            break;
        }
    }
    //outs() << "started\n";
    
    
    return PreservedAnalyses::none();
}