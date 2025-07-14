#include "scheduler.h"
#include "../../include/shared/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include "process.h"

// Filas de prioridade globais
static ProcessQueue priority_queues[NUMBER_OF_PRIORITY_QUEUES];
static ProcessQueue background_queue;
static ProcessQueue suspended_queue;
static int scheduler_initialized = 0;

// Inicializar o escalonador
void init_scheduler() {
    for (int i = 0; i < NUMBER_OF_PRIORITY_QUEUES; i++) {
        priority_queues[i].front = 0;
        priority_queues[i].rear = -1;
        priority_queues[i].count = 0;
    }
    background_queue.front = 0;
    background_queue.rear = -1;
    background_queue.count = 0;

    suspended_queue.front = 0;
    suspended_queue.rear = -1;
    suspended_queue.count = 0;
    
    scheduler_initialized = 1;
}

// FIFO para processos tempo real
PCB* schedule_real_time_process() {
    if (!scheduler_initialized) init_scheduler();

    ProcessQueue* rt_queue = &priority_queues[PRIORITY_REAL_TIME];

    if (rt_queue->count == 0) return NULL;

    // Remover o primeiro processo da fila (FIFO)
    PCB* process = rt_queue->processes[rt_queue->front];
    rt_queue->front = (rt_queue->front + 1) % MAX_PROCESSES_PER_QUEUE;
    rt_queue->count--;

    process->state = PROCESS_RUNNING;
    return process;
}

// Algoritmo de escalonamento para processos de usuário
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

// Movendo os processos para a fila de background
void demote_user_process(PCB* process) {
    if (!process || process->priority == 0) return;

    printf("-> Quantum esgotado para PID %d. Movendo para background.\n", process->pid);
    process->state = PROCESS_BACKGROUND;
    process->time_in_background = 0; // Reseta o contador

    // Adicionando à fila de background
    if (background_queue.count < MAX_PROCESSES_PER_QUEUE) {
        background_queue.rear = (background_queue.rear + 1) % MAX_PROCESSES_PER_QUEUE;
        background_queue.processes[background_queue.rear] = process;
        background_queue.count++;
    }
}

// Gerar a transição de background para suspenso
void manage_background_processes() {
    if (background_queue.count == 0) return;

    int current_size = background_queue.count;
    for (int i = 0; i < current_size; i++) {
        PCB* process = background_queue.processes[background_queue.front];
        background_queue.front = (background_queue.front + 1) % MAX_PROCESSES_PER_QUEUE;
        background_queue.count--;

        process->time_in_background++;

        if (process->time_in_background > BACKGROUND_TIME_LIMIT) {
            // Antes de suspender, liberta os recursos de E/S do processo.
            printf("-> Limite de tempo em background atingido para PID %d. Liberando recursos antes de suspender.\n", process->pid);
            release_all_resources(process);

            // Agora, suspende o processo
            process->state = PROCESS_SUSPENDED;
            
            if (suspended_queue.count < MAX_PROCESSES_PER_QUEUE) {
                suspended_queue.rear = (suspended_queue.rear + 1) % MAX_PROCESSES_PER_QUEUE;
                suspended_queue.processes[suspended_queue.rear] = process;
                suspended_queue.count++;
            }
        } else {
            // Se não for suspenso, volta para o fim da fila de background
            background_queue.rear = (background_queue.rear + 1) % MAX_PROCESSES_PER_QUEUE;
            background_queue.processes[background_queue.rear] = process;
            background_queue.count++;
        }
    }
}


// TODO: [Pessoa 1] Implementar aging para evitar starvation
void apply_aging() {
    // Promove processos mais antigos nas filas de usuário para evitar starvation
    // Percorre prioridades baixas para mais altas
    for (int priority = PRIORITY_USER_3; priority > PRIORITY_USER_1; priority--) {
        ProcessQueue* queue = &priority_queues[priority];
        if (queue->count > 0) {
            // Remove o processo mais antigo (FIFO)
            PCB* process = queue->processes[queue->front];
            queue->front = (queue->front + 1) % MAX_PROCESSES_PER_QUEUE;
            queue->count--;
            // Aumenta prioridade (valor numérico menor)
            process->priority = (ProcessPriority)(priority - 1);
            // Reinsere na fila de maior prioridade
            add_process_to_queue(process);
        }
    }
}

// TODO: [Pessoa 1] Verificar se fila está vazia
int is_queue_empty(int priority) {
    if (priority < 0 || priority >= NUMBER_OF_PRIORITY_QUEUES) return 1;
    return priority_queues[priority].count == 0;
}