#include "memory.h"
#include <pthread.h>

MemoryManager mem_manager;

// --- Funções Internas (Não usam mutex) ---

static int _allocate_memory_unsafe(int num_blocks, int region) {
    int start = (region == 1) ? 0 : REAL_TIME_MEMORY_BLOCKS;
    int end = (region == 1) ? REAL_TIME_MEMORY_BLOCKS : TOTAL_MEMORY_BLOCKS;

    for (int i = start; i <= end - num_blocks; i++) {
        int ok = 1;
        for (int j = 0; j < num_blocks; j++) {
            if (mem_manager.blocks[i + j]) {
                ok = 0;
                break;
            }
        }
        if (ok) {
            for (int j = 0; j < num_blocks; j++) {
                mem_manager.blocks[i + j] = 1;
            }
            return i; // Retorna o offset
        }
    }
    return -1; // Falha
}

static void _free_memory_unsafe(int offset, int num_blocks) {
    for (int i = offset; i < offset + num_blocks && i < TOTAL_MEMORY_BLOCKS; i++) {
        mem_manager.blocks[i] = 0;
    }
}


// --- Funções Públicas (Lidam com o mutex) ---

void initialize_memory(MemoryManager *mgr) {
    mgr->total_blocks = TOTAL_MEMORY_BLOCKS;
    mgr->reserved_blocks = REAL_TIME_MEMORY_BLOCKS;
    mgr->shared_blocks = USER_MEMORY_BLOCKS;
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        mgr->blocks[i] = 0;
    }
    // Inicializa o mutex
    pthread_mutex_init(&mgr->lock, NULL);
}

void destroy_memory_manager() {
    pthread_mutex_destroy(&mem_manager.lock);
}

int allocate_memory(int num_blocks, int region) {
    pthread_mutex_lock(&mem_manager.lock); // Tranca o mutex
    int offset = _allocate_memory_unsafe(num_blocks, region); // Chama a função interna
    pthread_mutex_unlock(&mem_manager.lock); // Destranca
    return offset;
}

void free_memory(int offset, int num_blocks) {
    pthread_mutex_lock(&mem_manager.lock);     // Tranca
    _free_memory_unsafe(offset, num_blocks);  // Chama a função interna
    pthread_mutex_unlock(&mem_manager.lock); // Destranca
}

int check_memory_availability(int num_blocks, int region) {
    pthread_mutex_lock(&mem_manager.lock); // Tranca o mutex

    int is_available = 0; // Inicia não disponível
    
    // Tenta alocar usando a função interna
    int offset = _allocate_memory_unsafe(num_blocks, region);
    
    if (offset >= 0) {
        // Se aloca há espaço.
        // Desfaz a alocação em segiuda
        _free_memory_unsafe(offset, num_blocks);
        is_available = 1; // coloca como disponível
    }

    pthread_mutex_unlock(&mem_manager.lock); // Destranca mutx
    return is_available;
}