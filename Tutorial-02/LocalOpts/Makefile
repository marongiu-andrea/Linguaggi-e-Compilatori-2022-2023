OPTIMIZER := libTestPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config --cxxflags --ldflags --libs) -fPIC
LDFLAGS += $(shell llvm-config --libs --ldflags --system-libs)

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	arch -arm64 $(CXX) -dylib -shared $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
