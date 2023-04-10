#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <map>

using namespace llvm;

void optimizeInstMul(Instruction&);
void optimizeInstDiv(Instruction&);
void optimize(ConstantInt*, Value*, Instruction&);


bool StrengthReductionPass::runOnBasicBlock(BasicBlock &BB) {
  // Itera attraverso le istruzioni del basic block.
  for(auto iter_inst = BB.begin(); iter_inst != BB.end(); ++iter_inst) {

    // Se l'operazione ha un operatore binario.
    if (auto *op = dyn_cast<BinaryOperator>(&*iter_inst)) {
      std::string opcd = iter_inst->getOpcodeName();
      // Mappa per fare lo switch tra gli operatori.
      std::map<std::string, int> pairings = {
        {"mul", 1},
        {"udiv", 2}
      };
      switch (pairings[opcd]) {
        case 1:
					optimizeInstMul(I);
          break;
        case 2:
					optimizeInstDiv(I);
          break;
        default:
          break;
      }
    }
  }

  return true;
}


void optimizeInstMul(Instruction& I) {
  ConstantInt *opLeft = dyn_cast<ConstantInt>(I.getOperand(0));
  ConstantInt *opRight = dyn_cast<ConstantInt>(I.getOperand(1));

  // Se il primo operatore è costante ed il secondo no.
  if (opLeft && !opRight) {
    optimize(opLeft, I.getOperand(1), I);
  // Se il secondo operatore è costante ed il primo no.
  } else if (!opLeft && opRight) {
    optimize(opRight, I.getOperand(0), I);
  }

  // Altrimenti, non fa nulla.
  return;
}


void optimize(ConstantInt *op1, Value *op2, Instruction &I) {
  // Esegue un'ottimizzazione del tipo:
  // 15 * x = x * 15 = x << 4 - x
  // op1 -> costante, op2 -> x
  APInt val = op1->getValue();
  APInt incrementedVal = val.operator++();

  // Se l'operatore costante + 1 è una potenza del 2.
  if (incrementedVal.isPowerOf2()) {
    auto opType = op1->getType();
    auto newOperand = ConstantInt::get(opType, incrementedVal.exactLogBase2());
    Instruction *shift = BinaryOperator::Create(Instruction::Shl, op2, newOperand);
    shift->insertAfter(&I);
    Instruction *sub = BinaryOperator::Create(Instruction::Sub, shift, op2);
    sub->insertAfter(shift);
    I.replaceAllUsesWith(sub);
  }

  // Altrimenti, non fa nulla.
  return;
}


void optimizeInstDiv(Instruction& I) {
	ConstantInt *opRight = dyn_cast<ConstantInt>(I.getOperand(1));

  // Se il secondo operando è una costante.
  if (opRight) {
    APInt val = opRight->getValue();

    // Se il secondo operando è una potenza del 2.
    if (val.isPowerOf2()) {
      // Sostituisco con uno shift destro.
      auto opType = opRight->getType();
      auto newOperand = ConstantInt::get(opType, val.exactLogBase2());
      Instruction *NewInst = BinaryOperator::Create(Instruction::LShr, I.getOperand(0), newOperand);
      NewInst->insertAfter(&I);
      I.replaceAllUsesWith(NewInst);
    }
  }

	return;
}


bool StrengthReductionPass::runOnFunction(Function &F) {
  bool Transformed = false;

  for (auto Iter = F.begin(); Iter != F.end(); ++Iter) {
    if (runOnBasicBlock(*Iter)) {
      Transformed = true;
    }
  }

  return Transformed;
}


PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
  for (auto Iter = M.begin(); Iter != M.end(); ++Iter) {
    if (runOnFunction(*Iter)) {
      return PreservedAnalyses::none();
    }
  }

  return PreservedAnalyses::none();
}

