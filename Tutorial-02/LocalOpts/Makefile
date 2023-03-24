PASSES := transform,algebraic-identity,strength-reduction,multi-instruction-operations
TO_OPTIMIZE_SRC := test/test.ll
OPTIMIZED_BYTECODE := ./test.optimized.bc
OPTIMIZED_LL := ./test.optimized.ll

OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) $(OPTIMIZED_LL)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(OPTIMIZED_BYTECODE): $(OPTIMIZER)
	opt-$(LLVM_VERSION) -load-pass-plugin=./$(OPTIMIZER) -passes="$(PASSES)" $(TO_OPTIMIZE_SRC) -o $(OPTIMIZED_BYTECODE)

$(OPTIMIZED_LL): $(OPTIMIZED_BYTECODE)
	llvm-dis-$(LLVM_VERSION) $(OPTIMIZED_BYTECODE) -o $(OPTIMIZED_LL)

run: clean-optimized $(OPTIMIZED_LL)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)

clean-optimized:
	$(RM) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)
