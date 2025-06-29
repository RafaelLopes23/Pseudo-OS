// fileops.c
// TODO: [Pessoa 3] Implementar funções de operações de arquivo
// Funcionalidades a implementar:
// - Criar arquivo
// - Deletar arquivo
// - Ler arquivo
// - Escrever em arquivo

#include "fileops.h"
#include "filesystem.h"
#include <string.h>

int create_file(const char *filename, int blocks) {
    if (strlen(filename) > MAX_FILENAME_LEN) return -1;
    if (blocks <= 0 || blocks > check_disk_space()) return -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, filename, MAX_FILENAME_LEN);
            fs.files[i].block_count = blocks;
            fs.files[i].first_block = fs.used_blocks;
            fs.used_blocks += blocks;
            return 0;
        }
    }
    return -1;  // Nenhum slot disponível
}

int delete_file(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            fs.used_blocks -= fs.files[i].block_count;
            memset(&fs.files[i], 0, sizeof(File));
            return 0;
        }
    }
    return -1;  // Arquivo não encontrado
}

int read_file(const char *filename, char *buffer, int size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            size_t bytes_to_copy = (size < BLOCK_SIZE) ? size : BLOCK_SIZE;
            memcpy(buffer, fs.disk_blocks[fs.files[i].first_block], bytes_to_copy);
            return 0;
        }
    }
    return -1;
}

int write_file(const char *filename, const char *data, int size) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            size_t bytes_to_copy = (size < BLOCK_SIZE) ? size : BLOCK_SIZE;
            memcpy(fs.disk_blocks[fs.files[i].first_block], data, bytes_to_copy);
            return 0;
        }
    }
    return -1;
}



/*
// Função para criar um arquivo
// TODO: [Pessoa 3] Implementar a lógica para criar um arquivo
int create_file(const char *filename, int blocks) {
    // Implementar aqui: lógica para criar um arquivo no sistema de arquivos
    return 0;  // placeholder
}

// Função para deletar um arquivo
// TODO: [Pessoa 3] Implementar a lógica para deletar um arquivo
int delete_file(const char *filename) {
    // Implementar aqui: lógica para deletar um arquivo do sistema de arquivos
    return 0;  // placeholder
}

// Função para ler um arquivo
// TODO: [Pessoa 3] Implementar a lógica para ler um arquivo
int read_file(const char *filename, char *buffer, int size) {
    // Implementar aqui: lógica para ler um arquivo do sistema de arquivos
    return 0;  // placeholder
}

// Função para escrever em um arquivo
// TODO: [Pessoa 3] Implementar a lógica para escrever em um arquivo
int write_file(const char *filename, const char *data, int size) {
    // Implementar aqui: lógica para escrever em um arquivo no sistema de arquivos
    return 0;  // placeholder
}