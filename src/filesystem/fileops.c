#include "fileops.h"
#include "filesystem.h"
#include <string.h>
#include <stdio.h>
#include "../core/log.h"
#include <stdlib.h>


APFS fs; // Variável global do sistema de arquivos

// Cria um arquivo com o dono definido
// Retorna 0 em caso de sucesso, ou um código de erro em caso de falha

int create_file(const char *filename, int blocks, int owner_pid) {
    if (strlen(filename) > MAX_FILENAME_LEN) {
        printf("[ERRO] Nome do arquivo excede o limite de %d caracteres\n", MAX_FILENAME_LEN);
        return -1;
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, filename, MAX_FILENAME_LEN);
            fs.files[i].block_count = blocks;
            fs.files[i].first_block = fs.used_blocks;
            fs.files[i].owner_pid = owner_pid;  // Define o dono
            fs.used_blocks += blocks;
            return 0;
        }
    }
    return -1;  // Nenhum slot disponível

}

// Modifiquei as funções para incluir mensagens detalhadas
int delete_file(const char *filename, int requester_pid) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
             // Verificação de dono
            if (fs.files[i].owner_pid != requester_pid) {
                printf("[ERRO] Permissão negada: PID %d não é dono de '%s' (dono: PID %d)\n",
                       requester_pid, filename, fs.files[i].owner_pid);
                log_operation_result(requester_pid, "deletar", filename, 0);
                return -2;  // Código de erro para permissão negada
            }
            fs.used_blocks -= fs.files[i].block_count;
            memset(&fs.files[i], 0, sizeof(File));
            printf("[SUCESSO] Arquivo '%s' deletado (liberados %d blocos)\n", 
                   filename, fs.files[i].block_count);
            log_operation_result(requester_pid, "sucesso", filename, 1);
            return 0; // Sucesso
        }
    }
    printf("[ERRO] Arquivo '%s' não encontrado. Possíveis causas:\n"
           "- Nome incorreto\n"
           "- Arquivo já deletado\n"
           "- Sem Permissões\n", filename);
    // Log de erro
    log_operation_result(requester_pid, "falha-remover", filename, 0);
    return -1;  
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

// Cria arquivo inicial usando posição fixa e verifica sobreposição
int create_initial_file(const char *filename, uint32_t first_block, uint32_t blocks) {
    if (strlen(filename) > MAX_FILENAME_LEN || blocks == 0 || first_block + blocks > TOTAL_BLOCKS) {
        return -1;
    }
    // Verifica sobreposição com arquivos existentes
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            uint32_t start = fs.files[i].first_block;
            uint32_t count = fs.files[i].block_count;
            if (!(first_block + blocks <= start || first_block >= start + count)) {
                return -1;
            }
        }
    }
    // Achar slot livre e inserir arquivo
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, filename, MAX_FILENAME_LEN);
            fs.files[i].first_block = first_block;
            fs.files[i].block_count = blocks;
            fs.used_blocks += blocks;
            return 0;
        }
    }
    return -1;
}

