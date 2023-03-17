OPTIMIZER := libTestPass.so
OBJs := $(subst .cpp,.o,$(wildcard lib/*.cpp))

SRC_DIR := test
IR_DIR := ir

SRCs := $(wildcard $(SRC_DIR)/*.c)
IR0s := $(SRCs:$(SRC_DIR)/%.c=$(IR_DIR)/%.ll.O0)
IR2s := $(SRCs:$(SRC_DIR)/%.c=$(IR_DIR)/%.ll.O2)

CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC

all: $(IR0s) $(IR2s) $(OPTIMIZER)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(IR_DIR)/%.ll.O0: $(SRC_DIR)/%.c | $(IR_DIR)
	clang -O0 -emit-llvm -S -c $< -o $@

$(IR_DIR)/%.ll.O2: $(SRC_DIR)/%.c | $(IR_DIR)
	clang -O2 -emit-llvm -S -c $< -o $@

$(IR_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs) $(IR_DIR)/*
