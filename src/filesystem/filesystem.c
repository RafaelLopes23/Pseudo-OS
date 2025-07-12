#include "filesystem.h"
#include "fileops.h"
#include <string.h>
#include <stdio.h>
#include "../core/log.h"

APFS fs;

void init_filesystem() {
    memset(&fs, 0, sizeof(fs));
    fs.used_blocks = 0;
    printf("Sistema de arquivos inicializado.\n");
}

int check_disk_space() {
    return TOTAL_BLOCKS - fs.used_blocks;
}


void list_files() {
    print_section_header("ARQUIVOS NO DISCO");
    
    char header[COL2_WIDTH];
    snprintf(header, COL2_WIDTH, "%-*s %*s %*s", 
             FILENAME_LEN, "NOME",
             NUM_COL_WIDTH, "BLOCOS",
             NUM_COL_WIDTH, "DONO");
    print_log_entry("TIPO", header);
    print_divider('-');

    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            char file_info[COL2_WIDTH];
            char safe_name[FILENAME_LEN+1];
            strncpy(safe_name, fs.files[i].name, FILENAME_LEN);
            safe_name[FILENAME_LEN] = '\0';
            
            snprintf(file_info, COL2_WIDTH, "%-*s %*d %*d", 
                     FILENAME_LEN, safe_name,
                     NUM_COL_WIDTH, fs.files[i].block_count,
                     NUM_COL_WIDTH, fs.files[i].owner_pid);
            print_log_entry("📄 ARQ", file_info);
        }
    }
    print_divider('=');
}



void check_disk_usage() {
    printf("Uso do disco: %d/%d blocos (%.1f%%)\n",
           fs.used_blocks, TOTAL_BLOCKS,
           (float)fs.used_blocks / TOTAL_BLOCKS * 100);
}

void print_disk_block_map() {
    printf("Mapa de ocupação do disco por bloco:\n");
    int blocks_per_line = 32;

    for (int blk = 0; blk < TOTAL_BLOCKS; blk++) {
        if (blk > 0 && blk % blocks_per_line == 0) {
            printf("\n");
        }

        int found = 0;
        for (int i = 0; i < MAX_FILES; i++) {
            if (fs.files[i].name[0] != '\0') {
                uint32_t start = fs.files[i].first_block;
                uint32_t count = fs.files[i].block_count;
                if ((uint32_t)blk >= start && (uint32_t)blk < start + count) {
                    printf("# "); 
                    found = 1;
                    break;
                }
            }
        }
        if (!found) {
            printf(". ");
        }
    }
    printf("\n");
}