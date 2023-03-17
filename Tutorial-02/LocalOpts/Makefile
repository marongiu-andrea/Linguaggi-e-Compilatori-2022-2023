TO_OPTIMIZE_SRC := test/test.ll
PASSES := transform
OPTIMIZED_BYTECODE := ./test_opt.bc
OPTIMIZED_LL := ./test_opt.ll

OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) $(OPTIMIZED_LL)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(OPTIMIZED_BYTECODE): $(OPTIMIZER)
	opt-14 -load-pass-plugin=./$(OPTIMIZER) -passes=$(PASSES) $(TO_OPTIMIZE_SRC) -o $(OPTIMIZED_BYTECODE)

$(OPTIMIZED_LL): $(OPTIMIZED_BYTECODE)
	llvm-dis-14 $(OPTIMIZED_BYTECODE) -o $(OPTIMIZED_LL)

run: clean-optimized $(OPTIMIZED_LL)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)

clean-optimized:
	$(RM) $(OPTIMIZED_BYTECODE) $(OPTIMIZED_LL)
