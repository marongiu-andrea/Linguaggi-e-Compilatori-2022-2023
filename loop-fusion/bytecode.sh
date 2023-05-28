src=test/Loop.c
dst=test/Loop.bc
clang -O0 -emit-llvm -c $src -o $dst

src=test/Loop.bc
dst=test/Loop.ll
llvm-dis $src -o=./$dst
