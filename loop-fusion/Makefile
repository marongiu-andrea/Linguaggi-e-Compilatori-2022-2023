OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 15

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

setup:
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c test/Loop.c -o test/Loop.bc
	opt -passes=mem2reg test/Loop.bc -o test/Loop.opt.bc
	llvm-dis test/Loop.opt.bc -o test/Loop.opt.ll

pass:
	opt -load-pass-plugin=./libLocalOpts.so -passes=LoopFusion ./test/Loop.opt.ll -o test/Loop.bc
	llvm-dis test/Loop.bc -o test/Loop.opt.ll

compile:
	llc test/Loop.opt.bc -filetype=obj -o test/Loop.opt.o
	llc test/Loop.bc -filetype=obj -o test/Loop.o
	gcc test/Loop.o test/main.c -o mainNoOpt
	gcc test/Loop.opt.o test/main.c -o mainOpt

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
