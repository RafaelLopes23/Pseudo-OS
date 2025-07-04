#ifndef MEMORY_H
#define MEMORY_H

#include "../../include/shared/constants.h"

typedef struct {
    int total_blocks;
    int reserved_blocks;
    int shared_blocks;
    int blocks[TOTAL_MEMORY_BLOCKS];  // bitmap de uso de cada bloco
} MemoryManager;

extern MemoryManager mem_manager;

// Inicializa todos os blocos como livres
void initialize_memory(MemoryManager *mgr);

// Aloca `blocks` blocos; `region`=1→tempo real, 0→usuário
// Retorna offset inicial ou -1 se falhar
int allocate_memory(int blocks, int region);

// Libera `blocks` blocos a partir de `offset`
void free_memory(int offset, int blocks);

// Checa disponibilidade sem alterar o estado
// Retorna 1 se houver espaço, 0 caso contrário
int check_memory_availability(int blocks, int region);

#endif // MEMORY_H