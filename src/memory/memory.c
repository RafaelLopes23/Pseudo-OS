// src/memory/memory.c
#include "memory.h"
#include "../../include/shared/constants.h"
// TODO: [Pessoa 2] Implementar gerenciador de memória (1024 blocos)
// TODO: [Pessoa 2] Implementar alocação contígua de memória

MemoryManager mem_manager;

// Função para inicializar o gerenciador de memória
void initialize_memory(MemoryManager *mem_manager) {
    // TODO: [Pessoa 2] Implementar inicialização do gerenciador de memória
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        mem_manager->blocks[i] = 0;  // Tudo livre no começo
    }
}

// Função para alocar blocos de memória
int allocate_memory(int num_blocks, int is_real_time) {
    int start, end;
    
    if (is_real_time) {
        start = 0;
        end = REAL_TIME_MEMORY_BLOCKS;
    } else {
        start = REAL_TIME_MEMORY_BLOCKS;
        end = TOTAL_MEMORY_BLOCKS;
    }

    for (int i = start; i <= end - num_blocks; i++) {
        int free = 1;
        for (int j = i; j < i + num_blocks; j++) {
            if (mem_manager.blocks[j] == 1) {
                free = 0;
                break;
            }
        }
        if (free) {
            for (int j = i; j < i + num_blocks; j++) {
                mem_manager.blocks[j] = 1;
            }
            return i;  // Retorna o offset inicial alocado
        }
    }

    return -1;  // Sem espaço suficiente
}


// Função para liberar blocos de memória
void free_memory(int start_block, int num_blocks) {
    for (int i = start_block; i < start_block + num_blocks; i++) {
        mem_manager.blocks[i] = 0;
    }
}


// Função para verificar disponibilidade de memória
int check_memory_availability(int num_blocks, int is_real_time) {
    int start, end;
    
    if (is_real_time) {
        start = 0;
        end = REAL_TIME_MEMORY_BLOCKS;
    } else {
        start = REAL_TIME_MEMORY_BLOCKS;
        end = TOTAL_MEMORY_BLOCKS;
    }

    for (int i = start; i <= end - num_blocks; i++) {
        int free = 1;
        for (int j = i; j < i + num_blocks; j++) {
            if (mem_manager.blocks[j] == 1) {
                free = 0;
                break;
            }
        }
        if (free) {
            return 1;  // Tem espaço disponível
        }
    }

    return 0;  // Não tem espaço
}
