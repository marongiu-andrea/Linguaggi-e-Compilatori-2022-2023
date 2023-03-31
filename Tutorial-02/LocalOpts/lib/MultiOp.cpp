#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

bool runOnBBMultiOP(BasicBlock &bb)
{
  std::list<Instruction*> toBeRemoved;

  Instruction* prev = nullptr;

  for(auto &inst: bb)
  {
    if(inst.isBinaryOp())
    {
        if(prev)
        {
            int prevOpcode = prev->getOpcode();
            int currOpcode = inst.getOpcode();

            bool isOptimizable = false;

            if(prevOpcode == Instruction::Mul && (currOpcode == Instruction::SDiv || currOpcode == Instruction::UDiv)
                || (currOpcode == Instruction::Mul && (prevOpcode == Instruction::SDiv || prevOpcode == Instruction::UDiv))
            )
            {
                outs() << "prev instr is mul/div\n";
                outs() << "inst: " << inst << "\n";

                isOptimizable = true;
            }
            else if(prevOpcode == Instruction::Add && currOpcode == Instruction::Sub 
                || prevOpcode == Instruction::Sub && currOpcode == Instruction::Add
            )
            {
                outs() << "prev instr is add/sub\n";
                outs() << "inst: " << inst << "\n";
                
                isOptimizable = true;
            }
            if(isOptimizable)
            {
                ConstantData* prevOpL = dyn_cast<ConstantData>(prev->getOperand(0));
                ConstantData* prevOpR = dyn_cast<ConstantData>(prev->getOperand(1));

                ConstantData* valPrev = nullptr;
                Value* keptPrev = nullptr;
                
                if(prevOpL && !prevOpR)
                {
                    valPrev = prevOpL;
                    keptPrev = prev->getOperand(1);
                }
                else if(prevOpR && !prevOpL)
                {
                    valPrev = prevOpR;
                    keptPrev = prev->getOperand(0);
                }

                ConstantData* currOpL = dyn_cast<ConstantData>(inst.getOperand(0));
                ConstantData* currOpR = dyn_cast<ConstantData>(inst.getOperand(1));

                ConstantData* valCurr = nullptr;
                Value* keptCurr = nullptr;

                if(currOpL && !currOpR)
                {
                    valCurr = currOpL;
                    keptCurr = inst.getOperand(1);
                }
                else if(currOpR && !currOpL)
                {
                    valCurr = currOpR;
                    keptCurr = inst.getOperand(0);
                }

                if(prev == keptCurr)
                {
                    outs() << "Optimization is possible\n";
                    
                    double prevOp;
                    double currOp;

                    if(ConstantInt* tmpInt = dyn_cast<ConstantInt>(valPrev))
                    {
                        prevOp = APFloat(APFloatBase::IEEEdouble(), tmpInt->getValue()).convertToDouble();
                    }
                    else       // valPrev is floating-point
                    {
                        prevOp = dyn_cast<ConstantFP>(valPrev)->getValue().convertToDouble();
                    }

                    if(ConstantInt* tmpInt = dyn_cast<ConstantInt>(valCurr))
                    {
                        currOp = APFloat(APFloatBase::IEEEdouble(), tmpInt->getValue()).convertToDouble();
                    }
                    else        // valCurr is floating-point
                    {
                        currOp = dyn_cast<ConstantFP>(valCurr)->getValue().convertToDouble();
                    }

                    Instruction* newInst = nullptr;
                    Value* newOperand = nullptr;
                    double result;
                    
                    if(currOpcode == Instruction::Mul)
                        result = currOp/prevOp;
                    else if(currOpcode == Instruction::SDiv || currOpcode == Instruction::UDiv)
                        result = prevOp/currOp;
                    else if(currOpcode == Instruction::Add)
                         result = currOp-prevOp;
                    else
                         result = currOp+prevOp;
                         
                    outs() << "Result: " << result << "\n";
                    
                    IntegerType* castToInt = dyn_cast<IntegerType>(prev->getType());

                    if(castToInt)
                    {
                        result = int(result);
                        newOperand = ConstantInt::get(castToInt, result);

                        outs() << "Operands are integers\n\n";
                    }
                    else
                    {
                        newOperand = ConstantFP::get(inst.getContext(), APFloat(result));
                        outs() << "Operands are floating-point\n\n";
                    }

                    if(currOpcode == Instruction::Mul || prevOpcode == Instruction::Mul)
                    {
                        if(abs(result) == 1)        // current instruction is useless
                            inst.replaceAllUsesWith(prev);
                        else if(abs(result) > 1)    // new inst is mul
                            newInst = BinaryOperator::Create(Instruction::Mul, keptPrev, newOperand);
                        else                        // new inst is div
                            newInst = BinaryOperator::Create(Instruction::SDiv, keptPrev, newOperand);   
                    }
                    else if(currOpcode == Instruction::Add || prevOpcode == Instruction::Add)
                    {
                        if(result == 0)             // current instruction is useless
                            inst.replaceAllUsesWith(prev);
                        else if(result > 0)         // new inst is mul
                            newInst = BinaryOperator::Create(Instruction::Add, keptPrev, newOperand);
                        else                        // new inst is div
                            newInst = BinaryOperator::Create(Instruction::Sub, keptPrev, newOperand);
                    }
                    if(newInst)
                    {
                        newInst->insertBefore(&inst);
                        inst.replaceAllUsesWith(newInst);
                    }
                    toBeRemoved.push_back(&inst);
                }
            }
        }
        prev = &inst;
    }
  }

  for(auto i: toBeRemoved)
  {
    i->eraseFromParent();
  }

  return true;
}


bool runOnFuncMultiOP(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBBMultiOP(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}




PreservedAnalyses MultiOpPass::run([[maybe_unused]] Module &M,
                                             ModuleAnalysisManager &) {

  // Un semplice passo di esempio di manipolazione della IR
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFuncMultiOP(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

