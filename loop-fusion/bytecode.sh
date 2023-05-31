clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/Loop.c -o test/Loop.bc
llvm-dis test/Loop.bc -o=./test/Loop.ll