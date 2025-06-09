// scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"  // Dependência do módulo de processos

// Estruturas e constantes para o escalonador
#define MAX_PRIORITY_LEVELS 4

// TODO: [Pessoa 1] Definir a estrutura de fila de processos
typedef struct {
    PCB* processes[MAX_PRIORITY_LEVELS][1000]; // Filas de prioridade
    int count[MAX_PRIORITY_LEVELS]; // Contagem de processos em cada fila
} PriorityQueue;

// Funções do escalonador
// TODO: [Pessoa 1] Implementar algoritmo FIFO para processos tempo real
int schedule_real_time_process();

// TODO: [Pessoa 1] Implementar algoritmo de escalonamento para processos de usuário
int schedule_user_process();

// TODO: [Pessoa 1] Implementar função para adicionar processo à fila
void add_process_to_queue(PCB* process);

// TODO: [Pessoa 1] Implementar função para remover processo da fila
PCB* remove_process_from_queue(int priority);

// TODO: [Pessoa 1] Implementar função para verificar se a fila está vazia
int is_queue_empty(int priority);

#endif // SCHEDULER_H