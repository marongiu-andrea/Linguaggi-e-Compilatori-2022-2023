#!/bin/bash
opt -enable-new-pm=0 -load ./libLoopInvariantCodeMotion.so -loop-invariant-code-motion test/Loop.ll -o test/LoopInvariantCodeMotion.optimized.bc
llvm-dis test/LoopInvariantCodeMotion.optimized.bc -o test/LoopInvariantCodeMotion.optimized.ll