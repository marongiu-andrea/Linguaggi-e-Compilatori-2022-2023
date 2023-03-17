
LIBDIR = lib
OPTIMIZER := ${LIBDIR}/Transform.so
SRCFILEs := $(wildcard ${LIBDIR}/*.cpp)
OBJFILEs := $(subst .cpp,.o, ${SRCFILEs})
# SRCFILEs := lib/Transform.cpp
# OBJFILEs := lib/Transform.o

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC 
LLVMFLAGS := $(shell llvm-config --ldflags --libs --system-libs)

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJFILEs)
	$(CXX) ${LLVMFLAGS} -dylib -shared $^ -o $@
	@if [ ! $(file) = "" ]; then \
		make load ${file}; \
	fi

lib/Transform.o: lib/Transform.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

lib/LocalOpts.o: lib/LocalOpts.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)

load:
	opt -load-pass-plugin=./lib/Transform.so -passes=transform $(file)  -o $(subst .ll,.bc,$(file))
	llvm-dis $(subst .ll,.bc,$(file)) -o testTransform/Optimized.ll