#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;


int getSign(APInt value) {
  return value.isZero()? 0 : (value.isNegative()? -1 : 1);
}

int64_t getSignedValueWithSaturation(APInt value) {
  // If we're dealing with a big num, truncate it with signed saturation
  if (value.getBitWidth() > sizeof(int64_t) * CHAR_BIT)
    value = value.truncSSat(sizeof(int64_t) * CHAR_BIT);

  // Then get the sign extended value
  return value.getSExtValue();
}

struct FactorDecomposition {
  int64_t mulAbs;
  uint shift;
  int16_t factorSign;
  int16_t mulSign;
};
FactorDecomposition decomposeFactor(APInt factor) {
  FactorDecomposition ans;

  // Store the sign of factor
  ans.factorSign = getSign(factor);

  // Safe and fast path for factor = 0
  if (ans.factorSign == 0) {
    ans.shift = 0;
    ans.mulAbs = 0;
    return ans;
  }

  // Calculate abs(factor) in place
  if (ans.factorSign < 0)
    factor.negate();

  uint shift = factor.nearestLogBase2();
  ans.shift = shift;

  // Create the minimum-sized APInt representing the nearest power of two
  APInt nearestPowerOfTwo = APInt::getOneBitSet(shift + 1, shift);

  ans.mulSign = getSign(nearestPowerOfTwo);
  ans.mulAbs = llabs(getSignedValueWithSaturation(factor - nearestPowerOfTwo));

  return ans;
}



struct OperandInfo {
  FactorDecomposition factorDecomp;
  Value* factor;
  Value* otherOperand;
};

static Optional<OperandInfo> getLowestMulOperand(Instruction& inst) {
  OperandInfo ans;

  bool ansValid = false;

  Value* op0 = inst.getOperand(0);
  Value* op1 = inst.getOperand(1);

  if (ConstantInt* c = dyn_cast<ConstantInt>(op0)) {
    ans.factorDecomp = decomposeFactor(c->getValue());
    ans.factor = op0;
    ans.otherOperand = op1;
    ansValid = true;
  }

  if (ConstantInt* c = dyn_cast<ConstantInt>(op1)) {
    FactorDecomposition op1fd = decomposeFactor(c->getValue());

    // If the first operand was not a constant or this constant's mulAbs factor is lower
    if (!ansValid || op1fd.mulAbs < ans.factorDecomp.mulAbs) {
      ans.factorDecomp = op1fd;
      ans.factor = op1;
      ans.otherOperand = op0;
      ansValid = true;
    }
  }

  if (ansValid)
    return ans;
  else
    return {};
}


#define MUL_FACTOR_THRESH 1

static void runOnBasicBlock(BasicBlock& bb) {
  auto ii = bb.begin();

  while (ii != bb.end()) {
    outs() << *ii << "\n";
    switch (ii->getOpcode()) {
      case Instruction::Mul:
        if (Optional<OperandInfo> opInfo = getLowestMulOperand(*ii)) {
          if (opInfo->factorDecomp.factorSign == 0) {
            // Multiplication by 0...
            // Replace all uses with the constant 0 and delete the instruction.
            ii->replaceAllUsesWith(opInfo->factor);
            ii = ii->eraseFromParent();
            break;

          } else if (opInfo->factorDecomp.mulAbs <= MUL_FACTOR_THRESH) {
            // The multiplication factor is low enough that it makes sense to apply strength reduction.

            Instruction& oldMulInst = *ii;

            IRBuilder<> builder(&(*ii));
            
            // ans <- otherOperand << shift
            Value* ans = builder.CreateShl(opInfo->otherOperand, opInfo->factorDecomp.shift);

            if (opInfo->factorDecomp.mulSign > 0) {
              // ans <- ans + otherOperand * mul

              for (int i = 0; i < opInfo->factorDecomp.mulAbs; ++i)
                ans = builder.CreateAdd(ans, opInfo->otherOperand);

            } else if (opInfo->factorDecomp.mulSign < 0) {
              // ans <- ans - otherOperand * mul

              for (int i = 0; i < opInfo->factorDecomp.mulAbs; ++i)
                ans = builder.CreateSub(ans, opInfo->otherOperand);
            }

            if (opInfo->factorDecomp.factorSign < 0) {
              // ans <- -ans = 0 -  ans
              ans = builder.CreateSub(ConstantInt::get(bb.getContext(), APInt(1, 0, false)), ans);
            }

            oldMulInst.replaceAllUsesWith(ans);
            oldMulInst.eraseFromParent();
            break;
          }
        }

        break;

      case Instruction::SDiv:
      case Instruction::UDiv:
        /*
        x = the first operand
        y = the second operand

        if y.mulAbs == 0:
          delete ans = x * y
          add ans = x >> y.shift

          if y.factorSign < 0:
            add ans = 0 - ans
        */
      
        break;
    }

    ++ii;
  }
}

PreservedAnalyses StrengthReductionPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "Strength Reduction\n";



  //FactorDecomposition dec = decomposeFactor(factor);
  //outs() << dec.mulAbs << ", " << dec.shift << ", " << dec.factorSign << "\n";

  for (auto bb = F.begin(); bb != F.end(); ++bb)
    runOnBasicBlock(*bb);

  return PreservedAnalyses::none();
}