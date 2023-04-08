OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))



CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC -I/opt/homebrew/opt/llvm@14/include
LDFLAGS := -L/opt/homebrew/opt/llvm@14/lib -lLLVM

all: $(OPTIMIZER)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)

format:
	clang-format -i -style=LLVM lib/*.cpp lib/*.h --sort-includes

