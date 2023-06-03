OPTIMIZER := libTestPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)

compila-loop:
	clang -O0 -emit-llvm -c test/Loop.c -o test/Loop.bc
	llvm-dis test/Loop.bc -o=./test/Loop.ll

compila-fibonacci:
	clang -O0 -emit-llvm -c test/Fibonacci.c -o test/Fibonacci.bc
	llvm-dis test/Fibonacci.bc -o=./test/Fibonacci.ll

ottimizza-loop:
	opt -load-pass-plugin=./libTestPass.so -passes=test-pass test/Loop.bc -o test/LoopTestPass.bc
	llvm-dis test/LoopTestPass.bc -o=./test/LoopTestPass.ll

ottimizza-fibonacci:
	opt -load-pass-plugin=./libTestPass.so -passes=test-pass test/Fibonacci.bc -o test/FibonacciTestPass.bc
	llvm-dis test/FibonacciTestPass.bc -o=./test/FibonacciTestPass.ll
