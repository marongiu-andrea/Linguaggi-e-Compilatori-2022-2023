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

ottimizza-algebraic:
	opt -load-pass-plugin=./libLocalOpts.so -passes=algebraic testTransform/Foo.ll -o testTransform/Foo.optimized-algebraic.bc
	llvm-dis testTransform/Foo.optimized-algebraic.bc -o testTransform/Foo.optimized-algebraic.ll

ottimizza-multiple:
	opt -load-pass-plugin=./libLocalOpts.so -passes=multiple testTransform/Foo.ll -o testTransform/Foo.optimized-multiple.bc
	llvm-dis testTransform/Foo.optimized-multiple.bc -o testTransform/Foo.optimized-multiple.ll

ottimizza-strength:
	opt -load-pass-plugin=./libLocalOpts.so -passes=strength testTransform/Foo.ll -o testTransform/Foo.optimized-strength.bc
	llvm-dis testTransform/Foo.optimized-strength.bc -o testTransform/Foo.optimized-strength.ll