OPTIMIZER := LICMPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 15

CXXFLAGS := $(shell llvm-config-$(LLVM_VERSION) --cxxflags) -fPIC

pass=loop-invariant-code-motion
optimizedbc=Loop.optimized.bc
optimizedll=Loop.optimized.ll

all: $(OPTIMIZER) 

pass:
	opt -enable-new-pm=0 -load ./LICMPass.so -${pass} test/Loop.ll -o ${optimizedbc}
	llvm-dis ${optimizedbc} -o ${optimizedll}


$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(OPTIMIZER) $(OBJs)
