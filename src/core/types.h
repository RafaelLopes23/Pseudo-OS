#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "../../include/shared/constants.h"

// Enum para prioridades de processos
typedef enum {
    PRIORITY_REAL_TIME = 0,
    PRIORITY_USER_1 = 1,
    PRIORITY_USER_2 = 2,
    PRIORITY_USER_3 = 3
} ProcessPriority;

// Estado dos processos
typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} ProcessState;

// Estrutura de Controle de Processo (PCB)
typedef struct {
    int pid;                     // Identificador do processo
    ProcessPriority priority;    // Prioridade do processo
    ProcessState state;          // Estado atual do processo
    uint32_t cpu_time;          // Tempo de CPU utilizado
    uint32_t total_cpu_time;    // Tempo total de CPU necessário
    uint32_t memory_blocks;      // Blocos de memória alocados
    int memory_offset;     // Offset da memória alocada
    int start_time;             // Tempo da inicialização

    // Recursos requisitados de E/S
    int printer_code;           // Código da impressora -> 0 = não usa
    int scanner_needed;         // Scanner necessário (0/1)
    int modem_needed;           // Modem necessário (0/1)
    int disk_code;              // Código do disco (0= não usa)

    int instruction_count;      // Contador de instruções executadas
    int quantum_remaining;      // Quantum restante (p/ processos usuário)
    bool has_started;           // Flag para indicar se o processo já foi escalonado
} PCB;

// Estrutura de fila de processos
typedef struct {
    PCB* processes[MAX_PROCESSES_PER_QUEUE]; // Array de ponteiros para processos
    int front;                      // Inicio da fila
    int rear;                       // Final da Fila
    int count;                      // Contador de processos na fila
} ProcessQueue;

#endif // TYPES_H