## produce Loop.ll
clang-14 -O2 -emit-llvm -c test/Loop.c -o test/Loop.bc
llvm-dis test/Loop.bc -o=./test/Loop.ll
## produce Fibonacci.ll
clang-14 -O2 -emit-llvm -c test/Fibonacci.c -o test/Fibonacci.bc
llvm-dis test/Fibonacci.bc -o=./test/Fibonacci.ll

make

# OPTIMIZE Loop.ll

# opt -load-pass-plugin=libTestPass.so -passes=test-pass -disable-output test/Loop.ll
opt -load-pass-plugin=./libTestPass.so -passes=test-pass test/Loop.ll -o test/Loop.opt.bc
llvm-dis test/Loop.opt.bc -o test/Loop.opt.ll

# OPTIMIZE Fibonacci.ll

# opt -load-pass-plugin=libTestPass.so -passes=test-pass -disable-output test/Fibonacci.ll
opt -load-pass-plugin=./libTestPass.so -passes=test-pass test/Fibonacci.ll -o test/Fibonacci.opt.bc
llvm-dis test/Fibonacci.opt.bc -o test/Fibonacci.opt.ll