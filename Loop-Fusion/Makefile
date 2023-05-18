# https://llvm.org/docs/NewPassManager.html#invoking-opt
PRE_PASSES_STRING := mem2reg,loop-rotate

PASSES_STRING := function(my-loop-fusion)

TO_OPTIMIZE := test/test
OPTIMIZED := test.optimized
BENCHMARK := test/bench

OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZED).ll

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(TO_OPTIMIZE).bc: $(TO_OPTIMIZE).c
	clang-$(LLVM_VERSION) $(TO_OPTIMIZE).c -c -Xclang -disable-O0-optnone -O0 -emit-llvm -o $(TO_OPTIMIZE).bc
	opt-$(LLVM_VERSION) $(TO_OPTIMIZE).bc -passes=$(PRE_PASSES_STRING) -o $(TO_OPTIMIZE).bc

$(TO_OPTIMIZE).ll: $(TO_OPTIMIZE).bc
	llvm-dis-$(LLVM_VERSION) $(TO_OPTIMIZE).bc -o $(TO_OPTIMIZE).ll

$(OPTIMIZED).bc: $(OPTIMIZER) $(TO_OPTIMIZE).ll
	opt-$(LLVM_VERSION) -load-pass-plugin=./$(OPTIMIZER) -passes="$(PASSES_STRING)" $(TO_OPTIMIZE).ll -o $(OPTIMIZED).bc

$(OPTIMIZED).ll: $(OPTIMIZED).bc
	llvm-dis-$(LLVM_VERSION) $(OPTIMIZED).bc -o $(OPTIMIZED).ll

$(OPTIMIZED).s: $(OPTIMIZED).bc
	llc-$(LLVM_VERSION) $(OPTIMIZED).bc -o $(OPTIMIZED).s

$(TO_OPTIMIZE).s: $(TO_OPTIMIZE).bc
	llc-$(LLVM_VERSION) $(TO_OPTIMIZE).bc -o $(TO_OPTIMIZE).s

run: clean-optimized $(OPTIMIZED).ll

.PHONY: clean clean-optimized benchmark

clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OPTIMIZED).bc $(OPTIMIZED).ll $(TO_OPTIMIZE).bc
	$(RM) $(BENCHMARK).o $(OPTIMIZED).s $(TO_OPTIMIZE).s

benchmark-optimized: $(BENCHMARK).c $(OPTIMIZED).s
	gcc $(BENCHMARK).c $(OPTIMIZED).s -o $(BENCHMARK).o
	./$(BENCHMARK).o

benchmark-non-optimized: $(BENCHMARK).c $(TO_OPTIMIZE).s
	gcc $(BENCHMARK).c $(TO_OPTIMIZE).s -o $(BENCHMARK).o
	./$(BENCHMARK).o

clean-optimized:
	$(RM) $(OPTIMIZED).bc $(OPTIMIZED).ll
