OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@


opty:
	opt -S -load-pass-plugin=./libLocalOpts.so -passes=transform testTransform/Foo.ll  -o testTransform/Foo.optimized.ll

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)


