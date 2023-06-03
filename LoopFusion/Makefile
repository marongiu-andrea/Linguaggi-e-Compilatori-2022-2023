OPTIMIZER := libLoopFusion.so
ARRAY_SIZE := 1000

BIN_DIR := bin
CFG_DIR := cfg
LIB_DIR := lib
OBJ_DIR := obj
TEST_DIR := test
STAT_DIR := stat

TESTFILE_SRC := $(TEST_DIR)/test.c
TESTFILE_LL := $(TESTFILE_SRC:%.c=%.ll)
TESTFILE_OPT_LL := $(TESTFILE_SRC:%.c=%.opt.ll)
TESTFILE_OBJ := $(TESTFILE_SRC:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)
TESTFILE_OPT_OBJ := $(TESTFILE_OBJ:%.o=%.opt.o)

POPULATE_CFG := $(CFG_DIR)/populate.svg
POPULATE_OPT_CFG := $(POPULATE_CFG:%.svg=%.opt.svg)

MAIN_SRC := $(TEST_DIR)/main.c
MAIN_OBJ := $(MAIN_SRC:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

LIB_SRC := $(LIB_DIR)/LoopFusion.cpp
LIB_INCLUDE := $(LIB_SRC:%.cpp=%.hpp)
LIB_OBJ := $(LIB_SRC:$(LIB_DIR)/%.cpp=$(OBJ_DIR)/%.o)

BIN := $(BIN_DIR)/main
BIN_OPT := $(BIN)-opt

STAT := $(STAT_DIR)/stat.json
STAT_OPT := $(STAT:%.json=%.opt.json)

CC = /usr/bin/clang
CXX = /usr/bin/clang++
OPT = /usr/bin/opt
DOT = /usr/bin/dot
LLC = /usr/bin/llc
PERF = /usr/bin/perf
CCFLAGS = -Wall -Werror -pedantic-errors
CXXFLAGS = $(shell llvm-config --cxxflags) $(CCFLAGS) -fPIC -std=c++20

all: $(TESTFILE_OPT_LL) $(POPULATE_CFG) $(POPULATE_OPT_CFG) $(BIN) $(BIN_OPT) $(STAT) $(STAT_OPT)

all-test: $(TESTFILE_OPT_LL) $(BIN) $(BIN_OPT) $(STAT) $(STAT_OPT)

$(TESTFILE_OPT_LL): $(TESTFILE_LL) $(OPTIMIZER)
	$(OPT) -load-pass-plugin=./$(OPTIMIZER) -passes=custom-loopfusion -S $< -o $@

$(TESTFILE_LL): $(TESTFILE_SRC)
	$(CC) -O0 -Xclang -disable-O0-optnone -emit-llvm -S $< -o $@ -DARRAY_SIZE=$(ARRAY_SIZE)
	$(OPT) -passes=mem2reg -S $@ -o $@

$(TESTFILE_OBJ): $(TESTFILE_LL)
	$(CC) -c $< -o $@

$(TESTFILE_OPT_OBJ): $(TESTFILE_OPT_LL)
	$(CC) -c $< -o $@

$(OPTIMIZER): $(LIB_OBJ)
	$(CXX) -dylib -shared $^ -o $@

$(LIB_OBJ): $(LIB_SRC) $(LIB_INCLUDE) | $(LIB_DIR) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(POPULATE_CFG): $(TESTFILE_LL) | $(CFG_DIR)
	$(OPT) -passes=dot-cfg $< >/dev/null 2>&1
	$(DOT) -Tsvg .populate.dot > $@
	$(RM) .populate.dot

$(POPULATE_OPT_CFG): $(TESTFILE_OPT_LL) | $(CFG_DIR)
	$(OPT) -passes=dot-cfg $< >/dev/null 2>&1
	$(DOT) -Tsvg .populate.dot > $@
	$(RM) .populate.dot

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CCFLAGS) -c $< -o $@ -DARRAY_SIZE=$(ARRAY_SIZE)

$(BIN): $(MAIN_OBJ) $(TESTFILE_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^

$(BIN_OPT): $(MAIN_OBJ) $(TESTFILE_OPT_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^

$(STAT): $(BIN) | $(STAT_DIR)
	$(PERF) stat -B -e cache-misses -j $< 2> $@

$(STAT_OPT): $(BIN_OPT) | $(STAT_DIR)
	$(PERF) stat -B -e cache-misses -j $< 2> $@

$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

$(CFG_DIR):
	mkdir -p $@

$(STAT_DIR):
	mkdir -p $@

.PHONY: clean clean-test
clean:
	$(RM) $(BIN_DIR)/* $(CFG_DIR)/* $(OBJ_DIR)/* $(OPTIMIZER) $(TEST_DIR)/*.ll $(STAT_DIR)/*

clean-test:
	$(RM) $(MAIN_OBJ) $(TESTFILE_OBJ) $(TESTFILE_OPT_OBJ) $(STAT_DIR)/* $(TESTFILE_LL) $(TESTFILE_OPT_LL)