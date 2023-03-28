OPTIMIZER := libLocalOpts.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

CXX = /usr/bin/clang++
CXXFLAGS = $(shell llvm-config --cxxflags) -fPIC -std=c++20 -Wall -Werror -pedantic-errors

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
