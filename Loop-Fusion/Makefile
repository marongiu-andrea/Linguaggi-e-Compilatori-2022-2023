# https://llvm.org/docs/NewPassManager.html#invoking-opt
PRE_PASSES_STRING := mem2reg,loop-rotate

PASSES_STRING := function(my-loop-fusion)

TO_OPTIMIZE_SRC_C := test/test.c
TO_OPTIMIZE_SRC := test/test.ll
TO_OPTIMIZE_BC := test/test.bc

OPTIMIZED_BYTECODE := test.optimized.bc
OPTIMIZED_LL := test.optimized.ll

OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZED_LL)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(TO_OPTIMIZE_SRC): $(TO_OPTIMIZE_SRC_C)
	clang-$(LLVM_VERSION) $(TO_OPTIMIZE_SRC_C) -c -Xclang -disable-O0-optnone -O0 -emit-llvm -o $(TO_OPTIMIZE_BC)
	opt-$(LLVM_VERSION) $(TO_OPTIMIZE_BC) -passes=$(PRE_PASSES_STRING) -o $(TO_OPTIMIZE_BC)
	llvm-dis-$(LLVM_VERSION) $(TO_OPTIMIZE_BC) -o $(TO_OPTIMIZE_SRC)

$(OPTIMIZED_BYTECODE): $(OPTIMIZER) $(TO_OPTIMIZE_SRC)
	opt-$(LLVM_VERSION) -load-pass-plugin=./$(OPTIMIZER) -passes="$(PASSES_STRING)" $(TO_OPTIMIZE_SRC) -o $(OPTIMIZED_BYTECODE)

$(OPTIMIZED_LL): $(OPTIMIZED_BYTECODE)
	llvm-dis-$(LLVM_VERSION) $(OPTIMIZED_BYTECODE) -o $(OPTIMIZED_LL)

run: clean-optimized $(OPTIMIZED_LL)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL) $(TO_OPTIMIZE_BC)

clean-optimized:
	$(RM) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)
