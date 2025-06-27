// memory.h
#ifndef MEMORY_H
#define MEMORY_H

// TODO: [Pessoa 2] Implementar gerenciamento de memória

// Estruturas de dados para gerenciamento de memória
typedef struct {
    int total_blocks;        // Total de blocos de memória
    int reserved_blocks;     // Blocos reservados para tempo real
    int shared_blocks;       // Blocos compartilhados para usuários
    int blocks[1024]; // 0 = livre, 1 = ocupado
} MemoryManager;

extern MemoryManager mem_manager;

// Funções para gerenciamento de memória
void initialize_memory(MemoryManager *mem_manager);
// TODO: [Pessoa 2] Implementar a função para inicializar o gerenciador de memória

int allocate_memory(int num_blocks, int is_real_time);
// TODO: [Pessoa 2] Implementar a função para alocar memória

void free_memory(int start_block, int num_blocks);
// TODO: [Pessoa 2] Implementar a função para liberar memória

// Funções para verificar disponibilidade de memória
int check_memory_availability(int num_blocks, int is_real_time);
// TODO: [Pessoa 2] Implementar a função para verificar a disponibilidade de memória

#endif // MEMORY_H