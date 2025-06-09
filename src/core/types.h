// types.h
// Responsável: Pessoa 1, Pessoa 2, Pessoa 3
// Funcionalidades a implementar:
// - Definição de tipos e estruturas comuns
// - Estruturas de dados compartilhadas (PCB, filas, etc.)
// - Constantes e macros centralizadas

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Definição de constantes
#define MAX_PROCESSES 1000
#define MEMORY_BLOCKS 1024
#define REAL_TIME_BLOCKS 64
#define USER_BLOCKS (MEMORY_BLOCKS - REAL_TIME_BLOCKS)

// Enum para prioridades de processos
typedef enum {
    PRIORITY_REAL_TIME = 0,
    PRIORITY_USER_1 = 1,
    PRIORITY_USER_2 = 2,
    PRIORITY_USER_3 = 3
} ProcessPriority;

// Estrutura de Controle de Processo (PCB)
typedef struct {
    int pid;                     // Identificador do processo
    ProcessPriority priority;    // Prioridade do processo
    uint32_t cpu_time;          // Tempo de CPU utilizado
    uint32_t memory_blocks;      // Blocos de memória alocados
    // TODO: [Pessoa 1] Adicionar outros campos necessários
} PCB;

// Estrutura de fila de processos
typedef struct {
    PCB* processes[MAX_PROCESSES]; // Array de ponteiros para processos
    int count;                      // Contador de processos na fila
    // TODO: [Pessoa 1] Adicionar funções para manipulação da fila
} ProcessQueue;

#endif // TYPES_H