#include "fileops.h"
#include "filesystem.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h> 
#include "../core/log.h"
#include <stdlib.h>



// Esta função remove espaços em branco e novas linhas do final de uma string
void sanitize_filename(char *filename) {
    int len = strlen(filename);
    while (len > 0 && isspace((unsigned char)filename[len - 1])) {
        len--;
    }
    filename[len] = '\0';
}

int create_file(const char *filename, int blocks, int owner_pid) {
    char clean_filename[MAX_FILENAME_LEN];
    strncpy(clean_filename, filename, MAX_FILENAME_LEN -1);
    clean_filename[MAX_FILENAME_LEN -1] = '\0';
    sanitize_filename(clean_filename);

    if (strlen(clean_filename) == 0) {
        log_operation_result(owner_pid, "criar", clean_filename, 0, "Nome do arquivo inválido");
        return -1;
    }
    if (strlen(clean_filename) > MAX_FILENAME_LEN) {
        log_operation_result(owner_pid, "criar", clean_filename, 0, "Nome do arquivo excede o limite");
        return -1;
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, clean_filename, MAX_FILENAME_LEN);
            fs.files[i].block_count = blocks;
            fs.files[i].first_block = fs.used_blocks;
            fs.files[i].owner_pid = owner_pid;
            fs.used_blocks += blocks;
            return 0;
        }
    }
    log_operation_result(owner_pid, "criar", clean_filename, 0, "Sistema de ficheiros cheio");
    return -1;
}

int delete_file(const char *filename, int requester_pid) {
    char clean_filename[MAX_FILENAME_LEN];
    strncpy(clean_filename, filename, MAX_FILENAME_LEN -1);
    clean_filename[MAX_FILENAME_LEN -1] = '\0';
    sanitize_filename(clean_filename);

    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, clean_filename) == 0) {
            if (fs.files[i].owner_pid != requester_pid) {
                char details[100];
                snprintf(details, sizeof(details), "Permissão Negada (Dono: PID %d)", fs.files[i].owner_pid);
                log_operation_result(requester_pid, "remover", clean_filename, 0, details);
                return -2;
            }

            int blocks_freed = fs.files[i].block_count;
            fs.used_blocks -= blocks_freed;
            memset(&fs.files[i], 0, sizeof(File));

            char success_details[50];
            snprintf(success_details, sizeof(success_details), "%d blocos liberados", blocks_freed);
            log_operation_result(requester_pid, "remover", clean_filename, 1, success_details);
            return 0;
        }
    }
    log_operation_result(requester_pid, "remover", clean_filename, 0, "Arquivo não encontrado");
    
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
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            uint32_t start = fs.files[i].first_block;
            uint32_t count = fs.files[i].block_count;
            if (!(first_block + blocks <= start || first_block >= start + count)) {
                return -1;
            }
        }
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            strncpy(fs.files[i].name, filename, MAX_FILENAME_LEN);
            fs.files[i].first_block = first_block;
            fs.files[i].block_count = blocks;
            fs.files[i].owner_pid = 0; 
            fs.used_blocks += blocks;
            return 0;
        }
    }
    return -1;
}

