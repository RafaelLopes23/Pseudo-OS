#include "filesystem.h"
#include "fileops.h"
#include <string.h>
#include <stdio.h>


APFS fs;

void init_filesystem(int disk_size){
    memset(&fs, 0, sizeof(fs));
    fs.total_blocks = disk_size;
    printf("Sistema de arquivos inicializado com %d blocos.\n", disk_size);
}

int check_disk_space(){
    return TOTAL_BLOCKS - fs.used_blocks;
}

void list_files() {
    printf("=== ARQUIVOS ===\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0' || fs.files[i].name[0] == '<') 
            continue;
        printf("%s (Blocos: %d)\n", fs.files[i].name, fs.files[i].block_count);
    }
}

void check_disk_usage() {
    printf("Uso do disco: %d/%u blocos (%.1f%%)\n", fs.used_blocks, fs.total_blocks,(float)fs.used_blocks / fs.total_blocks * 100);
}


void print_disk_block_map() {
    printf("Mapa de ocupação do disco por bloco:\n");

    // Cria um array temporário para armazenar o nome do arquivo em cada bloco
    char* block_owners[fs.total_blocks];
    for(uint32_t i = 0; i < fs.total_blocks; i++){
        block_owners[i] = "0"; // Inicializa todos como livres
    }

    // Preenche o array com os donos de cada bloco baseado na sua posição real
    for(int i = 0; i < MAX_FILES; i++){
        if(fs.files[i].name[0] != '\0') {
            for(uint32_t j = 0; j < fs.files[i].block_count; j++){
                uint32_t block_index = fs.files[i].first_block + j;
                if(block_index < fs.total_blocks){
                    block_owners[block_index] = fs.files[i].name;
                }
            }
        }
    }

    // Printa o mapa
    for (uint32_t i = 0; i < fs.total_blocks; i++) {
        printf("%s ", block_owners[i]);
    }
    printf("\n");
}