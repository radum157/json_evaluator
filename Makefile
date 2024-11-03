# Copyright 2024 Radu Marin

# Compiler options
CC := g++
FLAGS := -std=c++17 -Wall -Wextra -fPIC -I./include -O2
LIBFLAGS := -lpthread

# Directories
SRC_DIR := lib
OBJ_DIR := build

# Source files
SRC := main.cpp \
		lib/parser/parser.cpp \
		lib/evaluator/evaluator.cpp

# Object files
OBJS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

# Target executable
TARGET := json_eval

# Ensure the build directory exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Ensure the lib/parser and lib/evaluator directories exist
$(OBJ_DIR)/lib/parser:
	mkdir -p $(OBJ_DIR)/lib/parser

$(OBJ_DIR)/lib/evaluator:
	mkdir -p $(OBJ_DIR)/lib/evaluator

# Compile each source file to an object file
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)/lib/parser $(OBJ_DIR)/lib/evaluator
	$(CC) $(FLAGS) -c $< -o $@

# Link all object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(TARGET) $(LIBFLAGS)

.PHONY: build clean pack

build: $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ_DIR)

# Create ZIP archive
pack:
	zip -FSr json_eval.zip main.cpp README.md Makefile include/ lib/ test/ Dockerfile
