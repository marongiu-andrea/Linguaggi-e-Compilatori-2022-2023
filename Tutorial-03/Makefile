OPTIMIZER := libLoopWalk.so
OPTIMIZER_NEW := libLoopWalkNew.so
OBJs := LoopWalk.o
OBJs_NEW := LoopWalkNew.o

CXX = /usr/bin/clang++
CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC -std=c++20 -Wall -Werror -pedantic-errors

all: $(OPTIMIZER) $(OPTIMIZER_NEW)

$(OPTIMIZER): lib/LoopWalk.o
	$(CXX) -dylib -shared $^ -o $@

$(OPTIMIZER_NEW): lib/LoopWalkNew.o
	$(CXX) -dylib -shared $^ -o $@

lib/LoopWalk.o: lib/LoopWalk.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

lib/LoopWalkNew.o: lib/LoopWalkNew.cpp lib/LoopWalkNew.hpp
	$(CXX) $(CXXFLAGS) -c lib/LoopWalkNew.cpp -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
