# Makefile for the pseudo-operating system project

# --- Compilador e Flags ---
CC = gcc
# Adicionado -pthread para a linkagem e -g para debug
CFLAGS = -std=c99 -Wall -Wextra -g -I./include
LDFLAGS = -pthread

# --- Diretórios ---
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests

# --- Definições de Portabilidade (Windows vs. Outros) ---
# Detecta o sistema operacional e ajusta os comandos
ifeq ($(OS),Windows_NT)
    RM = del /Q /F
    EXE = .exe
    # Converte barras para o padrão do Windows no clean
    fix_path = $(subst /,\,$(1))
else
    RM = rm -f
    EXE =
    # Não faz nada em outros sistemas
    fix_path = $(1)
endif

# --- Arquivos-Fonte ---
CORE_SRC = $(SRC_DIR)/core/main.c
PROCESS_SRC = $(SRC_DIR)/process/process.c $(SRC_DIR)/process/scheduler.c $(SRC_DIR)/process/dispatcher.c
MEMORY_SRC = $(SRC_DIR)/memory/memory.c $(SRC_DIR)/memory/resources.c
FILESYSTEM_SRC = $(SRC_DIR)/filesystem/filesystem.c $(SRC_DIR)/filesystem/fileops.c

# --- Agrupamento de Arquivos-Objeto por Módulo ---
PROCESS_OBJ = $(PROCESS_SRC:.c=.o)
MEMORY_OBJ = $(MEMORY_SRC:.c=.o)
FILESYSTEM_OBJ = $(FILESYSTEM_SRC:.c=.o)
CORE_OBJ = $(CORE_SRC:.c=.o)

ALL_OBJ = $(CORE_OBJ) $(PROCESS_OBJ) $(MEMORY_OBJ) $(FILESYSTEM_OBJ)

# --- Alvo Principal ---
TARGET = pseudo-os$(EXE)

# Default: compila o programa principal
all: $(TARGET)

# --- Regras de Linkagem ---
# Adicionado $(LDFLAGS) para linkar com a biblioteca pthread
$(TARGET): $(ALL_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# --- Regra Genérica de Compilação ---
# Compila qualquer .c em um .o correspondente
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Regras de Teste (Corrigidas com Dependências) ---
test_process: $(PROCESS_OBJ) $(MEMORY_OBJ)
	$(CC) $(CFLAGS) -o test_process$(EXE) $(TEST_DIR)/test_process.c $(PROCESS_OBJ) $(MEMORY_OBJ) $(LDFLAGS)

test_memory: $(MEMORY_OBJ)
	$(CC) $(CFLAGS) -o test_memory$(EXE) $(TEST_DIR)/test_memory.c $(MEMORY_OBJ) $(LDFLAGS)

test_filesystem: $(FILESYSTEM_OBJ)
	$(CC) $(CFLAGS) -o test_filesystem$(EXE) $(TEST_DIR)/test_filesystem.c $(FILESYSTEM_OBJ) $(LDFLAGS)


# --- Regra de Limpeza (Portável) ---
clean:
	-$(RM) $(call fix_path, $(TARGET))
	-$(RM) $(call fix_path, test_process$(EXE))
	-$(RM) $(call fix_path, test_memory$(EXE))
	-$(RM) $(call fix_path, test_filesystem$(EXE))
	-$(RM) $(call fix_path, $(SRC_DIR)/*/*.o)
	-$(RM) $(call fix_path, $(SRC_DIR)/*/*/*.o)


# --- Alvos Falsos ---
.PHONY: all test_process test_memory test_filesystem clean