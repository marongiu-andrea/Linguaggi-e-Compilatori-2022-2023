#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <stack>
#include <math.h>

using namespace llvm;
bool AlgebraicIdentityPass::runOnBasicBlock(BasicBlock &BB) {
  // Itera attraverso le istruzioni del basic block.
  // Se trova un'istruzione che è:
  // - o una moltiplicazione/divisione in cui uno dei due operandi è 1;
  // - o una addizione/sottrazione in cui uno dei due operandi è 0;
  // sostituisce, in tutte le istruzioni successive, il registro destinazione
  // della istruzione "inutile" con l'altro operando dell'istruzione.
  // ad esempio: %5 = %4 + 0 --> sostituisco tutti gli usi di %5 con %4.
  for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {
    Instruction& I = *iter_inst;
    bool found;
    Value* PropOperand;
    std::string name = I.getOpcodeName();
    if(name == "add" || name == "mul" || name == "div" || name == "sub"){
      // check if one of the two operands is neutral element
      for (auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter) {        
        Value *Operand = *Iter;
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)){ // se trovo una costante
          if(name == "add" || name == "sub"){
            if(C->getValue() == llvm::APInt(32, 0)){ // se la costante è uno 0 e siamo in 
            // una addizione / sottrazione
              replaceUsesOfElement(Iter, I);
              if(Iter == I.op_begin()) PropOperand = *(++Iter);
              else {
                auto IterCopy = Iter;
                PropOperand = *(--IterCopy);
              }
              // old = lhs of instruction
              outs()<<"Replacing uses of: "<<I<<"\n";
              I.replaceAllUsesWith(PropOperand);              
            }
          }        
          if(name == "mul" || name == "div"){
            if(C->getValue() == llvm::APInt(32, 1)){
              if(Iter == I.op_begin()) PropOperand = *(++Iter);            
              else{
                auto IterCopy = Iter;
                PropOperand = *(--IterCopy);                              
              }
              outs()<<"Replacing uses of: "<<I<<"\n";
              I.replaceAllUsesWith(PropOperand);              
            }
          }
        }
      }
    }    
  }
  return true;
}


bool AlgebraicIdentityPass::runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
  // Un semplice passo di esempio di manipolazione della IR.
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

