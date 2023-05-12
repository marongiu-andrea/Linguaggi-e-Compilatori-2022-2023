
#include "LoopFusion.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/Analysis/LoopInfo.h>


#include <cmath>
#include <stdint.h>

using namespace llvm;

bool LoopFusionPass::runOnFunction(Function &f) {
    auto *loopInfoPass = &getAnalysis<LoopInfoWrapperPass>(f);
    auto &loopinfo = loopInfoPass->getLoopInfo();

    auto &sce = getAnalysis<ScalarEvolution>(f);

    auto loops = loopinfo.getLoopsInPreorder();

    for (int i = 0; i < loops.size() - 1; i++) {
        auto *eb = loops[i]->getExitBlock();

        if (!eb)
            continue;

        auto *preheader = loops[i + 1]->getLoopPreheader();
        if (loops[i]->getExitBlock() != preheader)
            continue;
        
        if (preheader->size() <= 1)
            continue;

        if (preheader->size() == 1) {
            Instruction *instr = &preheader->front();
            BranchInst *br = dyn_cast<BranchInst>(instr);
            if (!br || br->getNumSuccessors() != 1)
                continue;

            if (br->getSuccessor(0) != loops[i + 1]->getHeader())
                continue;
        }

        auto foptBounds = loops[i]->getBounds(sce);
        auto soptBounds = loops[i + 1]->getBounds(sce);

        if (!foptBounds.has_value() || !soptBounds.has_value())
            continue;

        auto fBounds = foptBounds.value();
        auto sBounds = soptBounds.value();

        // bound comparison 

    }

    return false;
}

void LoopFusionPass::getAnalysisUsage(AnalysisUsage &AU) const {
    //AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<LoopInfoWrapperPass>();
}

bool LoopFusionPass::runOnModule([[maybe_unused]] Module &M) {


    return false;
    /*
    // Un semplice passo di esempio di manipolazione della IR
    for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
        if (runOnFunction(*Iter))
            return PreservedAnalyses::none();
    }
    
    return PreservedAnalyses::none();*/
}

char LoopFusionPass::ID = 0;
RegisterPass<LoopFusionPass> X("loop-fusion",
                             "Loop Fusion");
