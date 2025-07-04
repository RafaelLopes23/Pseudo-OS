# Makefile for the pseudo-operating system project

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I./include
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
EXAMPLES_DIR = examples

# Windows commands
RM = del /Q /F
EXE = .exe

# Source files (apenas .c)
CORE_SRC = $(SRC_DIR)/core/main.c
PROCESS_SRC = $(SRC_DIR)/process/process.c $(SRC_DIR)/process/scheduler.c $(SRC_DIR)/process/dispatcher.c
MEMORY_SRC = $(SRC_DIR)/memory/memory.c $(SRC_DIR)/memory/resources.c
FILESYSTEM_SRC = $(SRC_DIR)/filesystem/filesystem.c $(SRC_DIR)/filesystem/fileops.c

# All source files
ALL_SRC = $(CORE_SRC) $(PROCESS_SRC) $(MEMORY_SRC) $(FILESYSTEM_SRC)

# Object files
OBJ = $(ALL_SRC:.c=.o)

# Target for the final executable
TARGET = pseudo-os$(EXE)

# Default target
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) -o $@ $^

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Test individual modules
test_process:
	$(CC) $(CFLAGS) -I./src -o test_process$(EXE) $(TEST_DIR)/test_process.c $(PROCESS_SRC)

test_memory:
	$(CC) $(CFLAGS) -I./src -o test_memory$(EXE) $(TEST_DIR)/test_memory.c $(MEMORY_SRC)

test_filesystem:
	$(CC) $(CFLAGS) -I./src -o test_filesystem$(EXE) $(TEST_DIR)/test_filesystem.c $(FILESYSTEM_SRC)

# Clean up build files (Windows)
clean:
	if exist *.o $(RM) *.o
	if exist *.exe $(RM) *.exe
	if exist $(TARGET) $(RM) $(TARGET)
	if exist test_process$(EXE) $(RM) test_process$(EXE)
	if exist test_memory$(EXE) $(RM) test_memory$(EXE)
	if exist test_filesystem$(EXE) $(RM) test_filesystem$(EXE)

# Phony targets
.PHONY: all test_process test_memory test_filesystem clean