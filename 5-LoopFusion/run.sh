# CREA Loop.ll da test.cc
clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -c test.cc -o test/Loop.bc
opt -passes=mem2reg test/Loop.bc -o test/Loop.base.bc
llvm-dis test/Loop.base.bc -o test/Loop.ll

# OTTIMIZZA Loop.ll
make
# opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -disable-output
opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -o test/Loop.fused.bc
llvm-dis test/Loop.fused.bc -o test/Loop.fused.ll

# Genera file eseguibile base
clang-14 -c -o test/Loop.base.o test/Loop.base.bc
clang-14 -o out/Loop.base.out test/Loop.base.o
./out/Loop.base.out

# Genera file eseguibile ottimizzato
clang-14 -c -o test/Loop.fused.o test/Loop.fused.bc
clang-14 -o out/Loop.fused.out test/Loop.fused.o
./out/Loop.fused.out