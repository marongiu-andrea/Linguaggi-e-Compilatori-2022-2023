
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
    return instr->getOpcode() == Instruction::SDiv ||
        instr->getOpcode() == Instruction::Sub;
}

static ConstantInt* GetConstOperand(Instruction* instr) {
    auto operand1 = instr->getOperand(0);
    auto operand2 = instr->getOperand(1);
    ConstantInt* op1Const = dyn_cast<ConstantInt>(operand1);
    ConstantInt* op2Const = dyn_cast<ConstantInt>(operand2);
    
    if(op1Const) return op1Const;
    if(op2Const) return op2Const;
    return 0;
}

static Value* GetVarOperand(Instruction* instr) {
    auto operand1 = instr->getOperand(0);
    auto operand2 = instr->getOperand(1);
    ConstantInt* op1Const = dyn_cast<ConstantInt>(operand1);
    ConstantInt* op2Const = dyn_cast<ConstantInt>(operand2);
    
    if(op1Const) return operand2;
    if(op2Const) return operand1;
    return 0;
}

static bool runOnInstruction(Instruction *instr) {
    static int count = 0;
    ++count;
    
    if(!instr->isBinaryOp())
        return false;
    
    int64_t oppositeOpcode = GetOppositeOpcode(instr);
    if(oppositeOpcode == -1)
        return false;
    
    ConstantInt *constOp = GetConstOperand(instr);
    auto operand1 = instr->getOperand(0);
    auto operand2 = instr->getOperand(1);
    
    if(!constOp) return false;
    
    // Ignore these cases: const / var, const - var
    if(!IsCommutative(instr) && constOp == operand1)
        return false;
    
    Instruction *defInstr = 0; 
    ConstantInt *opConst = 0;
    if(constOp == operand1)
        defInstr = dyn_cast<Instruction>(operand2);
    else
        defInstr = dyn_cast<Instruction>(operand1);
    
    // Analyze definition
    {
        if(!defInstr) return false;
        
        if(!defInstr->isBinaryOp())
            return false;
        
        ConstantInt *defConstOp = GetConstOperand(defInstr);
        Value *defVarOp = GetVarOperand(defInstr);
        auto defOpcode = defInstr->getOpcode();
        if(!defConstOp || !defVarOp) return false;
        
        // Ignore these cases: const / var, const - var
        if(!IsCommutative(defInstr) && defConstOp == defInstr->getOperand(0))
            return false;
        
        if(defOpcode == oppositeOpcode && defConstOp->getZExtValue() == constOp->getZExtValue()) {
            // Remove 'instr', and replace all of its uses
            instr->replaceAllUsesWith(defVarOp);
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
