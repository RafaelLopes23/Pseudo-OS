CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g -I./include -I./src/core
LDFLAGS = -pthread

SRC_DIR = src
OBJ_DIR = obj

# Encontra automaticamente todos os ficheiros .c nos subdiretórios de src
SOURCES = $(wildcard $(SRC_DIR)/*/*.c)

# Mapeia os ficheiros .c para os seus correspondentes ficheiros .o no diretório obj
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

TARGET = pseudo-os

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET) log.txt

.PHONY: all clean