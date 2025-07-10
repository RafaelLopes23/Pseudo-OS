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
#include <stdio.h>

int create_file(const char *filename, int blocks) {
    if (strlen(filename) >= MAX_FILENAME_LEN || blocks <= 0) return -1;

    int free_slot = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            free_slot = i;
            break;
        }
    }
    if (free_slot == -1) return -1; // Sem espaço na tabela de arquivos

    // Algoritmo First-Fit: Encontra o primeiro buraco grande o suficiente
    int consecutive_free = 0;
    int start_block = -1;
    for (uint32_t i = 0; i < fs.total_blocks; i++) {
        if (fs.block_map[i] == 0) {
            consecutive_free++;
        } else {
            consecutive_free = 0;
        }

        if (consecutive_free >= blocks) {
            start_block = i - blocks + 1;
            break;
        }
    }

    if (start_block == -1) return -1;

    //Aloca o arquivo
    strncpy(fs.files[free_slot].name, filename, MAX_FILENAME_LEN);
    fs.files[free_slot].first_block = start_block;
    fs.files[free_slot].block_count = blocks;

    //Att mapa
    for (int i = 0; i < blocks; i++) {
        fs.block_map[start_block + i] = 1;
    }
    fs.used_blocks += blocks;

    return 0;

}


int delete_file(const char *filename) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, filename) == 0) {
            // Libera os blocos no mapa de bits
            for (uint32_t j = 0; j < fs.files[i].block_count; j++) {
                fs.block_map[fs.files[i].first_block + j] = 0;
            }

            // Subtrai os blocos do total usado
            fs.used_blocks -= fs.files[i].block_count;

            // Limpa a entrada na tabela de arquivos
            memset(&fs.files[i], 0, sizeof(File));
            return 0;
        }
    }
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

int create_initial_file(const char *filename, uint32_t first_block, uint32_t blocks) {
    if (strlen(filename) >= MAX_FILENAME_LEN || blocks == 0) {
        fprintf(stderr, "Erro: Nome de arquivo ou tamanho de bloco inválido para %s.\n", filename);
        return -1;
    }

    //Flag para encontrar slot vazio
    int free_slot = -1;

    for(int i = 0; i < MAX_FILES; i++){
        if (fs.files[i].name[0] == '\0') {
            free_slot = i;
            break;
        }
    }

    if(free_slot == -1){
        fprintf(stderr, "Erro: Sem espaço na tabela de diretórios para criar %s.\n", filename);
        return -1; 
    }

    //Verifica se o arquivo cabe nos limites do disco
    if(first_block + blocks > fs.total_blocks){
        fprintf(stderr, "Erro: O arquivo %s (bloco %u, tamanho %u) excede o tamanho do disco (%u).\n", filename, first_block, blocks, fs.total_blocks);
        return -1;
    }

    //Verifica se os blocos estão livres usando o block_map
    for(uint32_t i = 0; i < blocks; i++){
        if(fs.block_map[first_block + i]!= 0){
            fprintf(stderr, "Erro: Sobreposição de blocos ao tentar criar %s no bloco %u.\n",
                    filename, first_block + i);
            return -1; // Sobreposição de arquivo
        }
    }
    
    // Copia os metadados para a tabela de arquivos
    strncpy(fs.files[free_slot].name, filename, MAX_FILENAME_LEN);
    fs.files[free_slot].first_block = first_block;
    fs.files[free_slot].block_count = blocks;

    // Atualiza a contagem total de blocos usados
    fs.used_blocks += blocks;

    // Marca os blocos como ocupados (1) no mapa de bits
    for(uint32_t i = 0; i < blocks; i++){
        fs.block_map[first_block + i] = 1;
    }

    return 0;
}