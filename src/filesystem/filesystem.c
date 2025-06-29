// TODO: [Pessoa 3] Implementar funções para gerenciamento do sistema de arquivos
// Responsável: Pessoa 3
// Funcionalidades a implementar:
// - Inicialização do sistema de arquivos
// - Criação e deleção de arquivos
// - Leitura e escrita em arquivos
// - Verificação de espaço disponível no disco
// Dependências: filesystem.h, fileops.h


#include "filesystem.h"
#include "fileops.h"
#include <string.h>
#include <stdio.h>

// Estrutura do APFS simulado
typedef struct {
    File files[MAX_FILES];
    uint8_t disk_blocks[TOTAL_BLOCKS][BLOCK_SIZE];
    int used_blocks;
} APFS;

static APFS fs;

// Inicialização do sistema de arquivos
void init_filesystem() {
    memset(&fs, 0, sizeof(APFS));
    fs.used_blocks = 0;
    printf("Sistema de arquivos inicializado.\n");
}

// Verifica espaço disponível
int check_disk_space() {
    return MAX_MEMORY_BLOCKS - fs.used_blocks;
}

// Lista arquivos
void list_files() {
    printf("=== ARQUIVOS ===\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] != '\0') {
            printf("%s (Blocos: %d)\n", fs.files[i].name, fs.files[i].block_count);
        }
    }
}

// Verifica ocupação do disco
void check_disk_usage() {
    printf("Uso do disco: %d/%d blocos (%.1f%%)\n",
           fs.used_blocks, TOTAL_BLOCKS,
           (float)fs.used_blocks / TOTAL_BLOCKS * 100);
}



/*

// Função para inicializar o sistema de arquivos
void init_filesystem() {
    // TODO: [Pessoa 3] Implementar a inicialização do sistema de arquivos
}

// Função para verificar espaço disponível no disco
int check_disk_space() {
    // TODO: [Pessoa 3] Implementar a verificação de espaço disponível
    return 0; // placeholder
}

// Função para listar arquivos no sistema de arquivos
void list_files() {
    // TODO: [Pessoa 3] Implementar a lógica para listar arquivos
}

// Função para verificar a ocupação do disco
void check_disk_usage() {
    // TODO: [Pessoa 3] Implementar a lógica para verificar a ocupação do disco
}