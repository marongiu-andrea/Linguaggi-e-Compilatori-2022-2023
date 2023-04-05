// #include "LocalOpts.h"
// #include "llvm/IR/InstrTypes.h"
// #include <map>

// using namespace llvm;

// std::map<uint32_t, uint32_t> mapping = {
//   std::make_pair(Instruction::Add, Instruction::Sub),
//   std::make_pair(Instruction::Sub, Instruction::Add)
// };

// bool check(
//   const ConstantInt*  first, 
//   const ConstantInt*  second, 
//   const uint32_t      type, 
//   const Instruction*  instr) 
// {
//   if(instr->getOpcode() == type && first->getValue() == second->getValue())
//     return true;
  
//   return false;
// }

// bool assign(ConstantInt*& constant, Value*& operand, Instruction* instr)
// {
//   ConstantInt* first = dyn_cast<ConstantInt>(instr->getOperand(0));
//   ConstantInt* second = dyn_cast<ConstantInt>(instr->getOperand(1));

//   if(first && second || !first && !second)
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

// bool multiInstrOpt(BasicBlock& BB)
// {
//   for(BasicBlock::iterator itBB = BB.begin(); itBB != BB.end(); ++itBB) 
//   {
//     Instruction& instr = *itBB;
//     const uint32_t opcode = instr.getOpcode();
    
//     ConstantInt*  firstConst;
//     ConstantInt*  secondConst;
//     Value*        firstInstOperand;
//     Value*        secondInstOperand;

//     if(mapping.find(opcode) == mapping.end())
//       mapping.insert({opcode, 0});

//     if(instr.isBinaryOp() && mapping.at(opcode)) 
//     {
//       if(!assign(firstConst, firstInstOperand, &instr))
//         continue;

//       for (auto itUser = instr.user_begin(); itUser != instr.user_end(); ++itUser) 
//       {
//         Instruction* user = dyn_cast<Instruction>(*itUser);
        
//         if(user->isBinaryOp() && mapping.at(opcode)) 
//         {
//           if(!assign(secondConst, secondInstOperand, user))
//             continue;
          
//           if(check(firstConst, secondConst, mapping.at(opcode), user)) 
//             user->replaceAllUsesWith(firstInstOperand);
//         }
//       }
//     }

//   }

//   return true;
// }

// static bool runOnFunction(Function& F) 
// {
//   bool transformed = false;

//   for (auto it = F.begin(); it != F.end(); ++it)
//   {
//     if (multiInstrOpt(*it)) 
//     {
//       transformed = true;
//     } 
//   }
//   return transformed;
// }

// PreservedAnalyses MultiInstructionPass::run([[maybe_unused]] Module &M,ModuleAnalysisManager &) 
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