#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <stdint.h>

// Constantes …
#define MAX_FILES       1000
#define TOTAL_BLOCKS    1024
#define BLOCK_SIZE      4096
#define MAX_FILENAME_LEN 255

typedef struct {
    char name[256];
    uint32_t first_block;
    uint32_t block_count;
} File;

// ====================== adicionar aqui ======================
typedef struct {
    File files[MAX_FILES];
    uint8_t disk_blocks[TOTAL_BLOCKS][BLOCK_SIZE];
    int used_blocks;
} APFS;
extern APFS fs;
// ============================================================

// Funções…
void init_filesystem();
int check_disk_space();
void list_files();
void check_disk_usage();
#endif // FILESYSTEM_H