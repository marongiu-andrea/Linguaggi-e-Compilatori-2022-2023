OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean setup
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
setup:
	opt -load-pass-plugin=./libLocalOpts.so -passes=algebraic $(firstrepr) -o $(bcfile)
	llvm-dis $(bcfile) -o $(optrepr)

.PHONY: auto
auto:
	make clean all setup firstrepr=./testTransform/Foo.ll bcfile=./testTransform/FooOpt.bc optrepr=./testTransform/FooOpt.ll