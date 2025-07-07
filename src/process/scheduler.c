#include "scheduler.h"
#include "../../include/shared/constants.h"
#include <stdio.h>
#include <stdlib.h>

// Filas de prioridade globais
static ProcessQueue priority_queues[NUMBER_OF_PRIORITY_QUEUES];
static int scheduler_initialized = 0;

// TODO: [Pessoa 1] Inicializar o escalonador
void init_scheduler() {
    for (int i = 0; i < NUMBER_OF_PRIORITY_QUEUES; i++) {
        priority_queues[i].front = 0;
        priority_queues[i].rear = -1;
        priority_queues[i].count = 0;
    }
    scheduler_initialized = 1;
}

// TODO: [Pessoa 1] Implementar algoritmo FIFO para processos tempo real
PCB* schedule_real_time_process() {
    if (!scheduler_initialized) init_scheduler();
    
    ProcessQueue* rt_queue = &priority_queues[PRIORITY_REAL_TIME];
    
    if (rt_queue->count == 0) return NULL;
    
    // Remove o primeiro processo da fila (FIFO)
    PCB* process = rt_queue->processes[rt_queue->front];
    rt_queue->front = (rt_queue->front + 1) % MAX_PROCESSES_PER_QUEUE;
    rt_queue->count--;
    
    process->state = PROCESS_RUNNING;
    return process;
}

// TODO: [Pessoa 1] Implementar algoritmo de escalonamento para processos de usuário
PCB* schedule_user_process() {
    if (!scheduler_initialized) init_scheduler();
    
    // Busca processo nas filas de usuário (prioridade 1, 2, 3)
    for (int priority = PRIORITY_USER_1; priority <= PRIORITY_USER_3; priority++) {
        ProcessQueue* queue = &priority_queues[priority];
        
        if (queue->count > 0) {
            PCB* process = queue->processes[queue->front];
            queue->front = (queue->front + 1) % MAX_PROCESSES_PER_QUEUE;
            queue->count--;
            
            process->state = PROCESS_RUNNING;
            process->quantum_remaining = USER_PROCESS_QUANTUM;
            return process;
        }
    }
    
    return NULL; // Nenhum processo disponível
}

// TODO: [Pessoa 1] Implementar função para adicionar processo à fila
int add_process_to_queue(PCB* process) {
    if (!process || !scheduler_initialized) return -1;
    
    int priority = process->priority;
    if (priority < 0 || priority >= NUMBER_OF_PRIORITY_QUEUES) return -1;
    
    ProcessQueue* queue = &priority_queues[priority];
    
    if (queue->count >= MAX_PROCESSES_PER_QUEUE) return -1; // Fila cheia
    
    queue->rear = (queue->rear + 1) % MAX_PROCESSES_PER_QUEUE;
    queue->processes[queue->rear] = process;
    queue->count++;
    
    process->state = PROCESS_READY;
    return 0;
}

// TODO: [Pessoa 1] Implementar realimentação para processos de usuário
void demote_user_process(PCB* process) {
    if (!process || process->priority == 0) return;
    
    // Diminui prioridade (aumenta o valor numérico)
    if (process->priority < PRIORITY_USER_3) {
        process->priority++;
    }
    
    // Adiciona de volta à fila com nova prioridade
    add_process_to_queue(process);
}

// TODO: [Pessoa 1] Implementar aging para evitar starvation
void apply_aging() {
    // Para implementação futura: promover processos antigos nas filas
    // Por simplicidade, pode ser implementado incrementalmente
}

// TODO: [Pessoa 1] Verificar se fila está vazia
int is_queue_empty(int priority) {
    if (priority < 0 || priority >= NUMBER_OF_PRIORITY_QUEUES) return 1;
    return priority_queues[priority].count == 0;
}