// src/process/process.h

#ifndef PROCESS_H
#define PROCESS_H

#include "../core/types.h"

// Estrutura do Bloco de Controle de Processo (PCB)
typedef struct {
    int pid;                // Identificador do processo
    int priority;          // Prioridade do processo
    int state;             // Estado do processo (pronto, em execução, bloqueado)
    int cpu_time;          // Tempo de CPU consumido
    int memory_blocks;     // Blocos de memória alocados
    // TODO: [Pessoa 1] Adicionar outros campos necessários para o PCB
} PCB;

// Funções relacionadas ao gerenciamento de processos
PCB* create_process(int priority, int memory_blocks); // Cria um novo processo
void destroy_process(PCB* process);                   // Destrói um processo existente
void print_process_info(PCB* process);                // Imprime informações do processo

// TODO: [Pessoa 1] Declarar outras funções necessárias para gerenciamento de processos

#endif // PROCESS_H