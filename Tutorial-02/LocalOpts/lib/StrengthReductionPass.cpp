#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

/**
 * @returns The sign of the parameter; equivalent to "value / abs(value)"
*/
int getSign(APInt value) {
  return value.isZero()? 0 : (value.isNegative()? -1 : 1);
}

/**
 * @returns The APInt value clamped to the int64 range, returned as an int64.
*/
int64_t clampToInt64(APInt value) {
  constexpr uint INT64_BITWIDTH = sizeof(int64_t) * CHAR_BIT;

  // If we're dealing with a big num, truncate it with signed saturation
  if (value.getBitWidth() > INT64_BITWIDTH)
    value = value.truncSSat(INT64_BITWIDTH);

  // Then get the sign extended value
  return value.getSExtValue();
}

struct FactorDecomposition {
  uint shift;

  // The original sign of the factor
  short factorSign;

  // The absolute value of the mul component.
  int64_t mulAbs;

  // The sign of the mul component.
  short mulSign;
};

/**
 * @returns The factor decomposition of factor, into a shift and mul component.
 *          For example, 14 will be decomposed into mul = -2 and shift = +4,
 *          because multiplying a value x by 14 is equivalent
 *          to (x << 4) - x * 2 = (x << 4) - x - x
*/
FactorDecomposition decomposeFactor(APInt factor) {
  FactorDecomposition ans;

  // Store the sign of factor
  ans.factorSign = getSign(factor);

  // Safe and fast path for factor = 0
  if (ans.factorSign == 0) {
    ans.shift = 0;
    ans.mulAbs = 0;
    ans.mulSign = 0;
    return ans;
  }

  // Calculate abs(factor) in place
  if (ans.factorSign < 0)
    factor.negate();

  uint shift = factor.nearestLogBase2();
  ans.shift = shift;

  // Create the minimum-sized APInt representing the nearest power of two
  APInt nearestPowerOfTwo = APInt::getOneBitSet(shift + 1, shift);
  APInt diff = factor - nearestPowerOfTwo;

  ans.mulSign = getSign(diff);
  ans.mulAbs = llabs(clampToInt64(diff));

  return ans;
}


/**
 * @brief The operands of an instruction:
 *        Identifies the (constant) factor and the other operand.
 *        Also contains the factor decomposition of the factor.
*/
struct OperandInfo {
  FactorDecomposition factorDecomp;
  Value* factor;
  Value* otherOperand;
};

/**
 * @brief In a commutative operation, identifies the (constant) factor with the factor decomposition having the lowest mul component.
*/
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
      // Then return this decomposition instead
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

/**
 * @brief Applies strength reduction to a mul operation.
 * @returns What the next analyzed instruction should be, or an empty optional if it did not change the flow of the analysis.
*/
static Optional<BasicBlock::iterator> mulStrenghtReduction(LLVMContext& ctx, Instruction& instr) {
  /* 
  MUL_FACTOR_THRESH is the maximum multiplier factor,
  aka the maximum amount of adds/subs to put after a shift to replace a mul.
  For example x * 14:
  with THRESH = 1, no strength reduction is applied.
  with THRESH = 2, becomes x << 4 - x - x
  */
  constexpr int MUL_FACTOR_THRESH = 1;

  if (Optional<OperandInfo> opInfo = getLowestMulOperand(instr)) {
    // At least one operand is a constant.

    if (opInfo->factorDecomp.factorSign == 0) {
      // Multiplication by 0...
      // Replace all uses with the constant 0 and delete the instruction.
      instr.replaceAllUsesWith(opInfo->factor);
      return instr.eraseFromParent();

    } else if (opInfo->factorDecomp.mulAbs <= MUL_FACTOR_THRESH) {
      // The multiplication factor is low enough that it makes sense to apply strength reduction.
      IRBuilder<> builder(&instr);
      
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

      // We also flip the result sign if the factor sign is negative.
      if (opInfo->factorDecomp.factorSign < 0) {
        // ans <- -ans = 0 -  ans
        ans = builder.CreateSub(ConstantInt::get(ctx, APInt(1, 0, false)), ans);
      }

      instr.replaceAllUsesWith(ans);
      instr.eraseFromParent();

      return ++BasicBlock::iterator(dyn_cast<Instruction>(ans));
    }
  }

  return {};
}

/**
 * @brief Applies strength reduction to a div operation.
 * @returns What the next analyzed instruction should be, or an empty optional if it did not change the flow of the analysis.
*/
static Optional<BasicBlock::iterator> divStrengthReduction(LLVMContext& ctx, Instruction& instr) {
  if (ConstantInt* val1 = dyn_cast<ConstantInt>(instr.getOperand(1))) {
    bool isUnsignedDiv = instr.getOpcode() == Instruction::UDiv;
    auto fd = decomposeFactor(val1->getValue());

    // Division can't be reduced unless the denominator is a power of two
    if (fd.mulSign != 0)
      return {};

    // Don't apply strength reduction for undefined cases:
    // division by 0
    // unsigned division with negative denominator
    if (fd.factorSign == 0 || (fd.factorSign < 0 && isUnsignedDiv))
      return {};
    
    // We apply strength reduction.
    IRBuilder<> builder(&instr);

    // ans <- op0 >> shift
    Value* ans = builder.CreateAShr(instr.getOperand(0), fd.shift);

    if (fd.factorSign < 0) {
        // ans <- -ans = 0 - ans
        ans = builder.CreateSub(ConstantInt::get(ctx, APInt(1, 0, false)), ans);
    }

    // Delete the old div instruction.
    instr.replaceAllUsesWith(ans);
    instr.eraseFromParent();

    return ++BasicBlock::iterator(dyn_cast<Instruction>(ans));
  }

  return {};
}

static void runOnBasicBlock(BasicBlock& bb) {
  auto ii = bb.begin();
  LLVMContext& ctx = bb.getContext();

  while (ii != bb.end()) {
    Optional<BasicBlock::iterator> newIi;

    /*
     Functions should return an empty optional
     if they do not change which instruction should be considered next,
     otherwise they should return a new iterator.
    */
    switch (ii->getOpcode()) {
      case Instruction::Mul:
        newIi = mulStrenghtReduction(ctx, *ii);
        break;

      case Instruction::SDiv:
      case Instruction::UDiv:
        newIi = divStrengthReduction(ctx, *ii);
    }

    if (newIi.hasValue())
      ii = *newIi;
    else
      ++ii;
  }
}

PreservedAnalyses StrengthReductionPass::run(Function &F,
                                             FunctionAnalysisManager &) {
  // TODO: Implement the pass
  outs() << "Strength Reduction\n";

  for (auto bb = F.begin(); bb != F.end(); ++bb)
    runOnBasicBlock(*bb);

  return PreservedAnalyses::none();
}