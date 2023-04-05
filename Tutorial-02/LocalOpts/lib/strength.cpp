// #include "LocalOpts.h"
// #include "llvm/IR/InstrTypes.h"

// using namespace llvm;


// static bool assign(ConstantInt*& constant, Value*& operand, Instruction* instr) 
// {
//   ConstantInt* first  = dyn_cast<ConstantInt>(instr->getOperand(0));
//   ConstantInt* second = dyn_cast<ConstantInt>(instr->getOperand(1));
  
//   if(!first && !second)
//     return false;

//   constant = second;
//   operand = instr->getOperand(0);

//   if(first) 
//   {
//     constant = first;
//     operand = instr->getOperand(1);
//   } 

//   return true;
// }

// static void insertInstruction(llvm::Instruction::BinaryOps type, Value* operand, int power, Instruction &Inst, int rest = 0) {
//   Instruction *NewInst = BinaryOperator::Create(
//     type,
//     operand,
//     ConstantInt::get(Inst.getContext() , APInt(32,  power))
//   );  
//   NewInst->insertAfter(&Inst);
//   Inst.replaceAllUsesWith(NewInst);
//   if(rest == -1) {
//     Instruction *Sub = BinaryOperator::Create(
//       Instruction::Sub,
//       NewInst,
//       operand
//     );  
//     Sub->insertAfter(NewInst);
//   } else if(rest == 1) {
//     Instruction *Add = BinaryOperator::Create(
//       Instruction::Add,
//       NewInst,
//       operand
//     );  
//     Add->insertAfter(NewInst);
//   }
// }

// bool strenghtReduction(BasicBlock &B) {
//   ConstantInt *constant;
//   Value *operand;
//   for(BasicBlock::iterator I = B.begin(); I != B.end(); ++I) {
//     Instruction &Inst = *I;
//     if(Inst.getOpcode() == Instruction::Mul) {
//       if(!assign(constant, operand, &Inst))
//         continue;
//       int power = constant->getValue().nearestLogBase2();
//       int rest = constant->getValue().roundToDouble(true) - pow(2, power);
//       if(abs(rest) <= 1)
//         insertInstruction(Instruction::Shl, operand, power, Inst, rest);

//     } else if(Inst.getOpcode() == Instruction::SDiv) {
//       constant = dyn_cast<ConstantInt>(Inst.getOperand(1));
//       if(constant && constant->getValue().exactLogBase2() > 0) {
//         insertInstruction(Instruction::AShr, Inst.getOperand(0),constant->getValue().exactLogBase2(), Inst);
//       }
//     }
//   }
//   return true;
// }




// static bool runOnFunction(Function &F) 
// {
//   bool transformed = false;

//   for (auto it = F.begin(); it != F.end(); ++it)
//   {
//     if (strenghtReduction(*it)) 
//     {
//       transformed = true;
//     } 
//   }
//   return transformed;
// }

// PreservedAnalyses TransformPass::run([[maybe_unused]] Module& M,ModuleAnalysisManager&) 
// {
//   // Un semplice passo di esempio di manipolazione della IR
//   for (auto it = M.begin(); it != M.end(); ++it) 
//   {
//     if (runOnFunction(*it)) 
//     {
//       return PreservedAnalyses::none();
//     }
//   }
//   return PreservedAnalyses::none();
// }
