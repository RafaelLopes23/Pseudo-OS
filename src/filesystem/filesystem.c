#include "filesystem.h"
#include "fileops.h"
#include <string.h>
#include <stdio.h>

// tirar static para ficar visível em fileops.c
APFS fs;

void init_filesystem() {
    memset(&fs, 0, sizeof(fs));
    fs.used_blocks = 0;
    printf("Sistema de arquivos inicializado.\n");
}

int check_disk_space() {
    // corrigido de MAX_MEMORY_BLOCKS para TOTAL_BLOCKS
    return TOTAL_BLOCKS - fs.used_blocks;
}

void list_files() {
    printf("=== ARQUIVOS ===\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            printf("%s (Blocos: %d)\n",
                   fs.files[i].name, fs.files[i].block_count);
        }
    }
}

void check_disk_usage() {
    printf("Uso do disco: %d/%d blocos (%.1f%%)\n",
           fs.used_blocks, TOTAL_BLOCKS,
           (float)fs.used_blocks / TOTAL_BLOCKS * 100);
}