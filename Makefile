# Makefile for the pseudo-operating system project

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I./include
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
EXAMPLES_DIR = examples

# choose rm or del based on OS
ifdef OS
  ifeq ($(OS),Windows_NT)
    RM = del /Q
  else
    RM = rm -f
  endif
else
  RM = rm -f
endif

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
TARGET = pseudo-os

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
	$(CC) $(CFLAGS) -o test_process.exe $(TEST_DIR)/test_process.c $(PROCESS_SRC)

# Clean up build files
clean:
	$(RM) $(OBJ) $(TARGET) test_process.exe

# Phony targets
.PHONY: all test_process clean