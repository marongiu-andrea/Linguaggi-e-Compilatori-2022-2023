LIBDIR = lib
OPTIMIZER := ${LIBDIR}/TestPass.so
SRCFILEs := $(wildcard ${LIBDIR}/*.cpp)
OBJFILEs := $(subst .cpp,.o, ${SRCFILEs})

LLVM_VERSION ?= 14

CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC 
LLVMFLAGS := $(shell llvm-config --ldflags --libs --system-libs)

all: $(OPTIMIZER) 

$(OPTIMIZER): $(OBJFILEs)
	$(CXX) ${LLVMFLAGS} -dylib -shared $^ -o $@
	@if [ ! $(file) = "" ]; then \
		make load ${file}; \
	fi

$(OBJFILEs): $(SRCFILEs)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJFILEs)

load:
	opt -load-pass-plugin=./lib/TestPass.so -passes=test-pass $(file) -disable-output

emit:
	clang++ -emit-llvm -S -c $(file) -o $(subst .c,.ll, ${file})

emit_opt:
	clang++ -O2 -emit-llvm -S -c $(file) -o $(subst .c,.ll, ${file})