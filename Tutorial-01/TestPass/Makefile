# https://godbolt.org/
OPTIMIZER := libTestPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))
OcHoM = loop fibonacci loop-zero fibonacci-zero # looprpass octy

LLVM_VERSION ?= 14

# funziona con la compilazione implicita
CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all:  $(OcHoM) opt

opt: $(OPTIMIZER)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@


# compila veloce e ottimizzato
loop: test/Loop.c
	clang -O2 -emit-llvm -S -c test/Loop.c -o test/Loop-ott.ll

looprpass : test/Loop.c
	clang -Rpass=test/.* -O2 -emit-llvm -S -c test/Loop.c -o test/Loop-rpass.ll

fibonacci: test/Fibonacci.c
	clang -O2 -emit-llvm -S -c test/Fibonacci.c -o test/Fibonacci-ott.ll 


loop-ott: test/Loop.c
	clang -O2 -emit-llvm -c test/Loop.c -o test/Loop.bc
	llvm-dis test/Loop.bc -o=./test/Loop-ott.ll

loop-zero: test/Loop.c
	clang -O0 -emit-llvm -c test/Loop.c -o test/Loop.bc
	llvm-dis test/Loop.bc -o=./test/Loop-zero.ll


fibonacci-ott: test/Fibonacci.c
	clang -O2 -emit-llvm -c test/Fibonacci.c -o test/Fibonacci.bc
	llvm-dis test/Fibonacci.bc -o=./test/Fibonacci-ott.ll

fibonacci-zero: test/Fibonacci.c
	clang -O2 -emit-llvm -c test/Fibonacci.c -o test/Fibonacci.bc
	llvm-dis test/Fibonacci.bc -o=./test/Fibonacci-zero.ll

octy:  libTestPass.so test/Loop-rpass.ll
	@#opt -S -load-pass-plugin=./libTestPass.so -passes=test-pass test/Loop-rpass.ll -o test/LoopTestPass.ll
	@#opt -S -load-pass-plugin=./libTestPass.so -passes=test-pass test/Fibonacci-zero.ll -o test/FibonacciTestPass.ll
	opt -S -load-pass-plugin=./libTestPass.so -passes=test-pass test/Foo.ll -o test/Foo-Passed.ll

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OcHoM)
