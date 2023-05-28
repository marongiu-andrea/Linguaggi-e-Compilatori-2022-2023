OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

opty:
	opt -S -load-pass-plugin=./$(OPTIMIZER) -passes=LoopFusion test/adjLoop.ll  -o test/adjLoopOptimized.ll

loop:
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c testPopulate/loop.cpp -o test/adjLoop.ll

compileobc:
	clang -shared -O0 -Xclang -disable-O0-optnone -emit-llvm -c testPopulate/loop.cpp -o test/adjLoop.bc
	opt -load-pass-plugin=./$(OPTIMIZER) -passes=LoopFusion test/adjLoopOptimized.ll -o test/adjLoopOptimized.bc
	llc test/adjLoopOptimized.bc -filetype=obj -o test/adjLoopOptimized.o
	llc test/adjLoop.bc -filetype=obj -o test/adjLoop.o
	gcc test/adjLoop.o main.cpp -o mainOutnonOpt
	gcc test/adjLoopOptimized.o main.cpp -o mainOutOpt





.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) test/adjLoopOptimized.ll
