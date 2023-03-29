#include "LocalOpts.h"
#include <llvm/IR/BasicBlock.h>

bool MultiInstrOptPass::runOnBasicBlock(llvm::BasicBlock& bb)
{
    return false;
}