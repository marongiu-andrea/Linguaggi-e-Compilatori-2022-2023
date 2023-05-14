OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)

setup:
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/Loop.c -o test/Loop.bc
	opt -passes=mem2reg test/Loop.bc -o test/Loop.opt.bc
	llvm-dis test/Loop.opt.bc -o Loop.ll

pass:
	opt -load-pass-plugin=./libLocalOpts.so -passes=loopfusion ./test/Loop.opt.ll -o Loop.bc
	llvm-dis Loop.bc -o Loop.opt.ll