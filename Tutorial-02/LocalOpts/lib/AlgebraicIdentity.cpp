#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <stack>
#include <math.h>
#include <map>

using namespace llvm;
bool AlgebraicIdentityPass::runOnBasicBlock(BasicBlock &BB) {
  // Itera attraverso le istruzioni del basic block.
  // Se trova un'istruzione che è:
  // - o una moltiplicazione in cui uno dei due operandi è 1;
  // - o una addizione in cui uno dei due operandi è 0;
  // sostituisce, in tutte le istruzioni successive, il registro destinazione
  // della istruzione "useless" con l'altro operando dell'istruzione.
  // ad esempio: %5 = %4 + 0 --> vengono sostituiti tutti gli usi di %5 con %4.
  for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {
    Instruction& I = *iter_inst;
    bool found;
    Value* PropOperand;
    std::string name = I.getOpcodeName();
    std::map<std::string, int> m; // hash map che memorizza l'elemento neutrale per ogni operazione di interesse
    m["add"] = 0;
    m["mul"] = 1;
    if(name == "add" || name == "mul"){
      // check if one of the two operands is neutral element
      for (auto *Iter = I.op_begin(); Iter != I.op_end(); ++Iter) {        
        Value *Operand = *Iter;
        if (ConstantInt *C = dyn_cast<ConstantInt>(Operand)){ // nel caso in cui si trova una costante        
          if(C->getValue() == llvm::APInt(32, m[name])){
            if(Iter == I.op_begin()) PropOperand = *(++Iter);
            else {
              auto IterCopy = Iter;
              PropOperand = *(--IterCopy);
            }
            I.replaceAllUsesWith(PropOperand);              
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

