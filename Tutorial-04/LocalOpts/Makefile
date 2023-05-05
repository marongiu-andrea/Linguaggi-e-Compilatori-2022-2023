OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard *.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) *.ll *.bc
setup:
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c Loop.c
	opt -passes=mem2reg Loop.bc -o Loop.opt.bc
	llvm-dis Loop.opt.bc -o Loop.opt.ll
pass:
	opt -load-pass-plugin=./libLocalOpts.so -passes=transform ./Loop.opt.ll -o ./Realout.bc
	llvm-dis ./Realout.bc -o ./Realout.ll


