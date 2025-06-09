# Makefile

# Makefile for the pseudo-operating system project

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
EXAMPLES_DIR = examples

# Source files
CORE_SRC = $(SRC_DIR)/core/main.c $(SRC_DIR)/core/system.h $(SRC_DIR)/core/types.h
PROCESS_SRC = $(SRC_DIR)/process/process.c $(SRC_DIR)/process/process.h $(SRC_DIR)/process/scheduler.c $(SRC_DIR)/process/scheduler.h $(SRC_DIR)/process/dispatcher.c
MEMORY_SRC = $(SRC_DIR)/memory/memory.c $(SRC_DIR)/memory/memory.h $(SRC_DIR)/memory/resources.c $(SRC_DIR)/memory/resources.h
FILESYSTEM_SRC = $(SRC_DIR)/filesystem/filesystem.c $(SRC_DIR)/filesystem/filesystem.h $(SRC_DIR)/filesystem/fileops.c $(SRC_DIR)/filesystem/fileops.h

# Object files
OBJ = $(CORE_SRC:.c=.o) $(PROCESS_SRC:.c=.o) $(MEMORY_SRC:.c=.o) $(FILESYSTEM_SRC:.c=.o)

# Target for the final executable
TARGET = pseudo-os

# Default target
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) -o $@ $^

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean

# Additional targets for testing
test: $(TEST_DIR)/test_process.c $(TEST_DIR)/test_memory.c $(TEST_DIR)/test_filesystem.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o test_process $(TEST_DIR)/test_process.c $(OBJ)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o test_memory $(TEST_DIR)/test_memory.c $(OBJ)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o test_filesystem $(TEST_DIR)/test_filesystem.c $(OBJ)

# Run tests
run_tests: test
	./test_process
	./test_memory
	./test_filesystem

# Example input files
examples:
	@echo "Example input files are located in $(EXAMPLES_DIR)"