#! /bin/bash

pass=loop-invariant-code-motion
optimizedbc=Loop.optimized.bc
optimizedll=Loop.optimized.ll
opt -enable-new-pm=0 -load ./LICMPass.so -$pass test/Loop.ll -o $optimizedbc
llvm-dis $optimizedbc -o $optimizedll
