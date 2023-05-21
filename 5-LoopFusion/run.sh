# RICREA Loop.ll
clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -c test.cpp -o test/Loop.bc
llvm-dis test/Loop.bc -o test/Loop.ll

# OTTIMIZZA Loop.ll
make
opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -disable-output
# opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/Loop.ll -o test/Loop.fused.bc
# llvm-dis test/Loop.fused.bc -o test/Loop.fused.ll
