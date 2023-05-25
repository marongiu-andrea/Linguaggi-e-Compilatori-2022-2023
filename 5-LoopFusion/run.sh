# CREA populate.base.ll da populate.cc
clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -c srcTest/populate.cc -o test/populate.bc
opt -passes=mem2reg test/populate.bc -o test/populate.base.bc
llvm-dis test/populate.base.bc -o test/populate.ll

# OTTIMIZZA populate.ll
make
# opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/populate.ll -disable-output
opt -enable-new-pm=0 -load ./libLoopFusion.so -loop-fusion-pass test/populate.ll -o test/populate.opt.bc
llvm-dis test/populate.opt.bc -o test/populate.opt.ll
echo -e
echo -e

# Genera file eseguibile base
clang-14 -c -o srcTest/populate.o test/populate.base.bc
clang-14 -c -o srcTest/main.o srcTest/main.cc
clang-14 srcTest/main.o srcTest/populate.o -o test.out -lstdc++ -std=c++11
./test.out
echo -e

# Genera file eseguibile ottimizzato
clang-14 -c -o srcTest/populate.o test/populate.opt.bc
clang-14 srcTest/main.o srcTest/populate.o -o test.opt.out -lstdc++ -std=c++11
./test.opt.out ottimizzato
echo -e

