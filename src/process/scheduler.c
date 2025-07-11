#include "scheduler.h"
#include "../../include/shared/constants.h"
#include <stdio.h>
#include <stdlib.h>

// Filas de prioridade globais
static ProcessQueue priority_queues[NUMBER_OF_PRIORITY_QUEUES];
static int scheduler_initialized = 0;

void init_scheduler() {
    for (int i = 0; i < NUMBER_OF_PRIORITY_QUEUES; i++) {
        priority_queues[i].front = 0;
        priority_queues[i].rear = -1;
        priority_queues[i].count = 0;
    }
    scheduler_initialized = 1;
}

PCB* schedule_real_time_process() {
    if (!scheduler_initialized) init_scheduler();

    ProcessQueue* rt_queue = &priority_queues[PRIORITY_REAL_TIME];

    if (rt_queue->count == 0) 
        return NULL;

    // Remove o primeiro processo da fila (FIFO)
    PCB* process = rt_queue->processes[rt_queue->front];
    rt_queue->front = (rt_queue->front + 1) % MAX_PROCESSES_PER_QUEUE;
    rt_queue->count--;

    process->state = PROCESS_RUNNING;
    return process;
}

PCB* schedule_user_process() {
    if (!scheduler_initialized) init_scheduler();

    // Busca processo na filas de usuario
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

int add_process_to_queue(PCB* process) {
    if (!process || !scheduler_initialized) 
        return -1;

    int priority = process->priority;
    if (priority < 0 || priority >= NUMBER_OF_PRIORITY_QUEUES) 
        return -1;

    ProcessQueue* queue = &priority_queues[priority];

    if (queue->count >= MAX_PROCESSES_PER_QUEUE) 
        return -1; // Fila cheia

    queue->rear = (queue->rear + 1) % MAX_PROCESSES_PER_QUEUE;
    queue->processes[queue->rear] = process;
    queue->count++;

    process->state = PROCESS_READY;
    return 0;
}

void demote_user_process(PCB* process) {
    if (!process || process->priority == 0) 
        return;

    //Diminui prioridade
    if (process->priority < PRIORITY_USER_3) {
        process->priority++;
    }

    //Adiciona de volta na fila
    add_process_to_queue(process);
}

//aging para evitar starvation
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
            // Aumenta prioridade
            process->priority = (ProcessPriority)(priority - 1);
            // Cola na fila novamente
            add_process_to_queue(process);
        }
    }
}

int is_queue_empty(int priority) {
    if (priority < 0 || priority >= NUMBER_OF_PRIORITY_QUEUES)
        return 1;
    return priority_queues[priority].count == 0;
}