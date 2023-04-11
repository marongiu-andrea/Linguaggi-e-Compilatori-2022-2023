
#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

#include <cmath>
#include <stdint.h>
#include <vector>

using namespace llvm;

inline int64_t GetOppositeOpcode(Instruction* instr)
{
    auto oppositeInstr = instr->getOpcode();
    switch(instr->getOpcode())
    {
        case Instruction::Mul:  oppositeInstr = Instruction::SDiv; break;
        case Instruction::SDiv: oppositeInstr = Instruction::Mul;  break;
        case Instruction::Add:  oppositeInstr = Instruction::Sub;  break;
        case Instruction::Sub:  oppositeInstr = Instruction::Add;  break;
        default: oppositeInstr = -1;
    }
    
    return oppositeInstr;
}

inline bool IsCommutative(Instruction* instr)
{
    switch(instr->getOpcode())
    {
        case Instruction::Mul:
        case Instruction::Add:
        return true;
        
        default: return false;
    }
    
    assert(false && "Unreachable code");
    return false;
}

// NOTE: works with non-constant operands (b = a+c, d = b-a => d=c)
static bool runOnInstruction(Instruction *instr) {
    // Only for debugging purposes
    static int count = -1;
    ++count;
    
    if(!instr->isBinaryOp())
        return false;
    
    int64_t oppositeOpcode = GetOppositeOpcode(instr);
    if(oppositeOpcode == -1)
        return false;
    
    auto operand1 = instr->getOperand(0);
    auto operand2 = instr->getOperand(1);
    
    Value *operands[] = { operand1, operand2 };
    
    // Analyze definitions
    for(int i = 0; i < 2; ++i) {
        auto toAnalyze = operands[i];
        auto otherOperand = operands[(i+1)%2];
        
        auto defInstr = dyn_cast<Instruction>(toAnalyze);
        
        if(!defInstr || !defInstr->isBinaryOp()) continue;
        if(defInstr->getOpcode() != oppositeOpcode) continue;
        
        auto defInstrOp1 = defInstr->getOperand(0);
        auto defInstrOp2 = defInstr->getOperand(1);
        
        bool isCommutative = IsCommutative(defInstr);
        Value* swapWith = 0;
        // Compare first operand only if the instruction is commutative
        if(isCommutative && otherOperand == defInstrOp1)
            swapWith = defInstrOp2;
        else if(otherOperand == defInstrOp2)
            swapWith = defInstrOp1;
        
        if(swapWith) {
            // Remove 'instr', and replace all of its uses
            instr->replaceAllUsesWith(swapWith);
            instr->eraseFromParent();
            return true;
        }
    }
    
    return false;
}

static bool runOnBlock(BasicBlock& block) {
    bool modified = false;
    Instruction *instr = &block.front();
    while(instr) {
        auto nextInstr = instr->getNextNode();
        modified |= runOnInstruction(instr);
        instr = nextInstr;
    }
    
    return modified;
}

static bool runOnFunction(Function& func) {
    for (auto it = func.begin(); it != func.end(); ++it) {
        if(runOnBlock(*it))
            return true;
    }
    
    return false;
}

PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,
                                            ModuleAnalysisManager &) {
    for (auto it = M.begin(); it != M.end(); ++it) {
        if (runOnFunction(*it))
            return PreservedAnalyses::none();
    }
    
    return PreservedAnalyses::none();
}
