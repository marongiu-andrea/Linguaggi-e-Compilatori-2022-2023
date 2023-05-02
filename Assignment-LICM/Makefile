PASSES := -loop-invariant-code-motion

TO_OPTIMIZE_SRC := test/Loop.ll
OPTIMIZED_BYTECODE := optimized.bc
OPTIMIZED_LL := optimized.ll

OPTIMIZER := ./libLICM.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZED_LL)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(OPTIMIZED_BYTECODE): $(OPTIMIZER)
	opt-$(LLVM_VERSION) -enable-new-pm=0 -load $(OPTIMIZER) $(PASSES) $(TO_OPTIMIZE_SRC) -o $(OPTIMIZED_BYTECODE)

$(OPTIMIZED_LL): $(OPTIMIZED_BYTECODE)
	llvm-dis-$(LLVM_VERSION) $(OPTIMIZED_BYTECODE) -o $(OPTIMIZED_LL)

run: clean-optimized $(OPTIMIZED_LL)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)

clean-optimized:
	$(RM) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)
