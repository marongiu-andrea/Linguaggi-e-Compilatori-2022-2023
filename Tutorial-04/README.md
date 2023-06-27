clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/loops/Loops.c -o test/loops/Loops.bc
opt -passes=mem2reg test/loops/Loops.bc -o test/loops/Loops.opt.bc
llvm-dis test/loops/Loops.opt.bc -o Loops.nonopt.ll
clang -O0 Loops.nonopt.ll -o nonopt.out

make
opt -enable-new-pm=0 -load=./libLoopFusion.so -fusion Loops.nonopt.ll -S -o Loops.opt.ll
clang -O0 Loops.opt.ll -o optimized.out