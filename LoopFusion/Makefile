OPTIMIZER := libLoopFusion.so
TESTFILE := test/test.ll

LIB_DIR := lib
OBJ_DIR := obj

SRCs := $(wildcard $(LIB_DIR)/*.cpp)
TESTFILE_SRC := $(TESTFILE:test/%.ll=test/%.c)
OBJs := $(SRCs:$(LIB_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = /usr/bin/clang
CXX = /usr/bin/clang++
CXXFLAGS = $(shell llvm-config --cxxflags) -fPIC -std=c++20 -Wall -Werror -pedantic-errors

all: $(OPTIMIZER) $(TESTFILE)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

$(TESTFILE): $(TESTFILE_SRC)
	$(CC) -O0 -Xclang -disable-O0-optnone -emit-llvm -S -c $< -o $@
	opt -passes=mem2reg -S $@ -o $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJ_DIR)/*.o $(TESTFILE)
