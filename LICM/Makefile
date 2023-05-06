OPTIMIZER := libLICM.so

LIB_DIR := lib
OBJ_DIR := obj

SRCs := $(wildcard $(LIB_DIR)/*.cpp)
INCLUDEs := $(SRCs:%.cpp=%.hpp)
OBJs := $(SRCs:$(LIB_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXX = /usr/bin/clang++
CXXFLAGS := $(shell llvm-config --cxxflags) -fPIC -std=c++20 -Wall -Werror -pedantic-errors

all: $(OPTIMIZER)

$(OPTIMIZER): $(OBJs)
	$(CXX) -dylib -shared $^ -o $@

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp $(LIB_DIR)/%.hpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	$(RM) $(OPTIMIZER) $(OBJs)
