#include "memory.h"

MemoryManager mem_manager;

void initialize_memory(MemoryManager *mgr) {
    mgr->total_blocks   = TOTAL_MEMORY_BLOCKS;
    mgr->reserved_blocks= REAL_TIME_MEMORY_BLOCKS;
    mgr->shared_blocks  = USER_MEMORY_BLOCKS;
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++)
        mgr->blocks[i] = 0;
}

int allocate_memory(int num_blocks, int region) {
    int start = (region == 1) ? 0 : REAL_TIME_MEMORY_BLOCKS;
    int end   = (region == 1) ? REAL_TIME_MEMORY_BLOCKS : TOTAL_MEMORY_BLOCKS;
    for (int i = start; i <= end - num_blocks; i++) {
        int ok = 1;
        for (int j = 0; j < num_blocks; j++) {
            if (mem_manager.blocks[i + j]) { ok = 0; break; }
        }
        if (ok) {
            for (int j = 0; j < num_blocks; j++)
                mem_manager.blocks[i + j] = 1;
            return i;
        }
    }
    return -1;
}

void free_memory(int offset, int num_blocks) {
    for (int i = offset; i < offset + num_blocks && i < TOTAL_MEMORY_BLOCKS; i++)
        mem_manager.blocks[i] = 0;
}

int check_memory_availability(int num_blocks, int region) {
    int off = allocate_memory(num_blocks, region);
    if (off >= 0) {
        free_memory(off, num_blocks);
        return 1;
    }
    return 0;
}