// filesystem.h
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

// TODO: [Pessoa 3] Implementar funções para gerenciamento do sistema de arquivos

#include <stdint.h>

// Estrutura que representa um arquivo no sistema de arquivos
typedef struct {
    char name[256];          // Nome do arquivo
    uint32_t first_block;    // Primeiro bloco alocado
    uint32_t block_count;     // Número de blocos alocados
} File;

// Função para inicializar o sistema de arquivos
void init_filesystem();

// Função para criar um arquivo
// TODO: [Pessoa 3] Implementar a lógica para criar um arquivo
int create_file(const char *filename, uint32_t block_count);

// Função para deletar um arquivo
// TODO: [Pessoa 3] Implementar a lógica para deletar um arquivo
int delete_file(const char *filename);

// Função para listar arquivos no sistema de arquivos
// TODO: [Pessoa 3] Implementar a lógica para listar arquivos
void list_files();

// Função para verificar a ocupação do disco
// TODO: [Pessoa 3] Implementar a lógica para verificar a ocupação do disco
void check_disk_usage();

#endif // FILESYSTEM_H