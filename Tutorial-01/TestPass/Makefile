OPTIMIZER := libTestPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

SRC_DIR := test
IR_DIR := ir
CFG_DIR := cfg

SRCs := $(wildcard $(SRC_DIR)/*.c)
IR0s := $(SRCs:$(SRC_DIR)/%.c=$(IR_DIR)/%.ll.O0)
IR2s := $(SRCs:$(SRC_DIR)/%.c=$(IR_DIR)/%.ll.O2)
CFGs := $(wildcard $(CFG_DIR)/src/*.d2)

CC = /usr/bin/clang
CXX = /usr/bin/clang++
D2 = /usr/bin/d2
CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC

all: $(IR0s) $(IR2s) $(OPTIMIZER) $(CFGs)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(IR_DIR)/%.ll.O0: $(SRC_DIR)/%.c | $(IR_DIR)
	$(CC) -O0 -emit-llvm -S -c $< -o $@

$(IR_DIR)/%.ll.O2: $(SRC_DIR)/%.c | $(IR_DIR)
	$(CC) -O2 -emit-llvm -S -c $< -o $@

$(CFG_DIR)/svg/%.svg: $(CFG_DIR)/src/%.d2
	$(D2) $< $@

$(IR_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(IR_DIR)/*
