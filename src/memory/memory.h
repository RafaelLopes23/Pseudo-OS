// memory.h
#ifndef MEMORY_H
#define MEMORY_H

// TODO: [Pessoa 2] Implementar gerenciamento de memória

// Estruturas de dados para gerenciamento de memória
typedef struct {
    int total_blocks;        // Total de blocos de memória
    int reserved_blocks;     // Blocos reservados para tempo real
    int shared_blocks;       // Blocos compartilhados para usuários
    // TODO: [Pessoa 2] Adicionar mais campos conforme necessário
} MemoryManager;

// Funções para gerenciamento de memória
void initialize_memory(MemoryManager *mem_manager);
// TODO: [Pessoa 2] Implementar a função para inicializar o gerenciador de memória

int allocate_memory(int blocks);
// TODO: [Pessoa 2] Implementar a função para alocar memória

void free_memory(int blocks);
// TODO: [Pessoa 2] Implementar a função para liberar memória

// Funções para verificar disponibilidade de memória
int check_memory_availability(int blocks);
// TODO: [Pessoa 2] Implementar a função para verificar a disponibilidade de memória

#endif // MEMORY_H