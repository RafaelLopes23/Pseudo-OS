#include "fileops.h"
#include "filesystem.h"
#include <string.h>
#include <stdio.h>
#include "../process/process.h"
#include "../core/system.h"

// Função para criar um arquivo usando o algoritmo First-Fit

void print_debug_string(const char* label, const char* str) {
    if (!str) {
        printf("DEBUG: %s -> (null)\n", label);
        return;
    }
    printf("DEBUG: %s -> '[%s]'\t(tamanho: %zu)\tASCII: ", label, str, strlen(str));
    for (size_t i = 0; i <= strlen(str); ++i) {
        printf("%d ", (int)str[i]);
    }
    printf("\n");
}


int create_file(const char *filename, int blocks, int owner_pid) {
    if (debug_mode) printf("--- DEBUG: Invocando create_file para '%s', tamanho %d, dono PID %d ---\n", filename, blocks, owner_pid);

    if (strlen(filename) >= MAX_FILENAME_LEN || blocks <= 0) {
        if (debug_mode) printf("DEBUG: Falha na validação inicial de parâmetros.\n");
        return -1;
    }

    //Procura um slot livre na tabela de arquivos
    int free_slot = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) {
        if (debug_mode) 
            printf("DEBUG: Falha - Tabela de arquivos está cheia.\n");
        return -1;
    }
    if (debug_mode) 
        printf("DEBUG: Slot livre encontrado na tabela de arquivos: %d\n", free_slot);

    //Algoritmo First-Fit
    int consecutive_free = 0;
    int start_block = -1;
    if (debug_mode) 
        printf("DEBUG: Iniciando busca First-Fit por %d blocos contíguos...\n", blocks);
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

    if (start_block == -1) {
        if (debug_mode) 
            printf("DEBUG: Falha - First-Fit não encontrou espaço contíguo suficiente.\n");
        return -1;
    }
    if (debug_mode) 
        printf("DEBUG: First-Fit encontrou espaço no bloco inicial %d.\n", start_block);

    //Se encontrou espaço aloca o arquivo
    if (debug_mode) 
        printf("DEBUG: Alocando arquivo '%s' no slot %d...\n", filename, free_slot);
    strncpy(fs.files[free_slot].name, filename, MAX_FILENAME_LEN);
    fs.files[free_slot].first_block = start_block;
    fs.files[free_slot].block_count = blocks;
    fs.files[free_slot].owner_pid = owner_pid;

    fs.used_blocks += blocks;
    for (int i = 0; i < blocks; i++) {
        fs.block_map[start_block + i] = 1;
    }

    if (debug_mode) 
        printf("DEBUG: Alocação concluída com sucesso.\n");
    return 0;
}

// Função para deletar um arquivo com verificação de permissão

int delete_file(const char *filename, int requester_pid, ProcessPriority requester_priority) {
    PCB* requester_process = get_process_by_pid(requester_pid);
    if (!requester_process) {
        return -1;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        // Encontra o arquivo pelo nome
        if (fs.files[i].name[0] != '\0' && strcmp(fs.files[i].name, filename) == 0) {
            if (requester_priority != PRIORITY_REAL_TIME && requester_process->pid != fs.files[i].owner_pid) {
                printf("Operação Falha: Processo %d não tem permissão para deletar o arquivo '%s'.\n", requester_pid, filename);
                return -1; // Falha
            }

            // Exclusão em si
            for (uint32_t j = 0; j < fs.files[i].block_count; j++) {
                fs.block_map[fs.files[i].first_block + j] = 0;
            }
            fs.used_blocks -= fs.files[i].block_count;
            memset(&fs.files[i], 0, sizeof(File));
            
            return 0; // Sucesso
        }
    }
    return -1; //Falha
}



int create_initial_file(const char *filename, uint32_t first_block, uint32_t blocks) {
    if (debug_mode) 
        printf("\n--- DEBUG: Invocando create_initial_file para '%s', bloco inicial %u, tamanho %u ---\n", filename, first_block, blocks);

    //Validação dos parâmetros
    if (strlen(filename) >= MAX_FILENAME_LEN || blocks == 0) {
        if (debug_mode) 
            printf("DEBUG: Falha na validação - nome do arquivo muito longo ou tamanho de bloco inválido.\n");
        return -1;
    }
    if (first_block + blocks > fs.total_blocks) {
        if (debug_mode) 
            printf("DEBUG: Falha na validação - arquivo excede os limites do disco (%u > %u).\n", first_block + blocks, fs.total_blocks);
        return -1;
    }

    //Procura um slot vazio na tabela de arquivos
    int free_slot = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (fs.files[i].name[0] == '\0') {
            free_slot = i;
            break;
        }
    }
    if (free_slot == -1) {
        if (debug_mode) 
            printf("DEBUG: Falha - Tabela de arquivos (fs.files) está cheia.\n");
        return -1;
    }
    if (debug_mode) 
        printf("DEBUG: Slot livre encontrado na tabela de arquivos no índice %d.\n", free_slot);

    //Verifica se os blocos solicitados já estão em uso (sobreposição)
    for (uint32_t i = 0; i < blocks; i++) {
        if (fs.block_map[first_block + i] != 0) {
            if (debug_mode) 
                printf("DEBUG: Falha - Sobreposição detectada no bloco %u.\n", first_block + i);
            return -1;
        }
    }
    if (debug_mode) 
        printf("DEBUG: Verificação de sobreposição OK. Blocos estão livres.\n");

    //Se tudo estiver OK, aloca o arquivo e atualiza as estruturas
    if (debug_mode) 
        printf("DEBUG: Alocando arquivo no slot %d...\n", free_slot);
    strncpy(fs.files[free_slot].name, filename, MAX_FILENAME_LEN);
    fs.files[free_slot].first_block = first_block;
    fs.files[free_slot].block_count = blocks;
    fs.files[free_slot].owner_pid = -1;

    fs.used_blocks += blocks;
    for (uint32_t i = 0; i < blocks; i++) {
        fs.block_map[first_block + i] = 1;
    }

    if (debug_mode) {
        print_debug_string("ARMAZENADO INICIALMENTE", fs.files[free_slot].name);
        printf("DEBUG: Alocação inicial de '%s' concluída com sucesso.\n", filename);
    }
    
    return 0;
}



// Funções de leitura
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

// Funções de escrita
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