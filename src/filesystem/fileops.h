// fileops.h
#ifndef FILEOPS_H

#define FILEOPS_H

// TODO: [Pessoa 3] Implementar operações de arquivo

#include "filesystem.h"

// Função para criar um arquivo
// Retorna 0 em caso de sucesso, ou um código de erro em caso de falha
int create_file(const char *filename, int blocks);

// Função para deletar um arquivo
// Retorna 0 em caso de sucesso, ou um código de erro em caso de falha
int delete_file(const char *filename);

// Função para ler um arquivo
// Retorna 0 em caso de sucesso, ou um código de erro em caso de falha
int read_file(const char *filename, char *buffer, int size);

// Função para escrever em um arquivo
// Retorna 0 em caso de sucesso, ou um código de erro em caso de falha
int write_file(const char *filename, const char *data, int size);

// Função para criação de arquivos iniciais em posição fixa no disco
int create_initial_file(const char *filename, uint32_t first_block, uint32_t blocks);
#endif // FILEOPS_H