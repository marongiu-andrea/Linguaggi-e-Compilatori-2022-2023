
LIBDIR = lib
SRCFILEs := $(wildcard ${LIBDIR}/*.cpp)
OBJFILEs := $(subst .cpp,.o, ${SRCFILEs})

STRENGTH := ${LIBDIR}/Strength.so
IDENTITY := ${LIBDIR}/AlgebricIdentity.so
MULTIINSTRUCTIONS := ${LIBDIR}/MultiInstructions.so

CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC 
LLVMFLAGS := $(shell llvm-config --ldflags --libs --system-libs)

all: $(STRENGTH) $(IDENTITY) $(MULTIINSTRUCTIONS)

$(STRENGTH): $(OBJFILEs)
	$(CXX) ${LLVMFLAGS} -dylib -shared $^ -o $@

$(IDENTITY): $(OBJFILEs)
	$(CXX) ${LLVMFLAGS} -dylib -shared $^ -o $@

$(MULTIINSTRUCTIONS): $(OBJFILEs)
	$(CXX) ${LLVMFLAGS} -dylib -shared $^ -o $@

.PHONY: clean
clean:
	$(RM) $(STRENGTH) $(IDENTITY) $(MULTIINSTRUCTIONS) $(OBJFILEs)

st:
	make $(STRENGTH)
	opt -load-pass-plugin=./lib/Strength.so -passes=strength $(file)  -o $(subst .ll,.bc,$(file))
	llvm-dis $(subst .ll,.bc,$(file)) -o testTransform/Optimized.ll

ai:
	make $(IDENTITY)
	opt -load-pass-plugin=./lib/AlgebricIdentity.so -passes=identity $(file)  -o $(subst .ll,.bc,$(file))
	llvm-dis $(subst .ll,.bc,$(file)) -o testTransform/Optimized.ll

mi:
	make $(MULTIINSTRUCTIONS)
	opt -load-pass-plugin=./lib/MultiInstructions.so -passes=multi $(file)  -o $(subst .ll,.bc,$(file))
	llvm-dis $(subst .ll,.bc,$(file)) -o testTransform/Optimized.ll