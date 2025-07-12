#include "semaphore.h"
#include "../process/scheduler.h"
#include <stdio.h>

// Inicializa um semáforo com um valor inicial
void init_semaphore(SimulatedSemaphore* sem, int initial_value) {
    sem->value = initial_value;
    // Inicializa a fila de espera específica deste semáforo
    sem->waiting_queue.front = 0;
    sem->waiting_queue.rear = -1;
    sem->waiting_queue.count = 0;
}

// Operação "wait" ou "P"
// Retorna 0 se o recurso foi obtido, 1 se o processo foi bloqueado
int sem_wait(SimulatedSemaphore* sem, PCB* process) {
    if (sem->value > 0) {
        // Recurso disponível, aloca
        sem->value--;
        return 0; // Sucesso, recurso alocado
    } else {
        // Recurso indisponível, bloqueia o processo
        process->state = PROCESS_BLOCKED;
        
        // Adiciona à fila de espera deste semáforo
        ProcessQueue* wq = &sem->waiting_queue;
        if (wq->count < MAX_PROCESSES_PER_QUEUE) {
            wq->rear = (wq->rear + 1) % MAX_PROCESSES_PER_QUEUE;
            wq->processes[wq->rear] = process;
            wq->count++;
        }
        return 1; // Processo bloqueado
    }
}

// Operação "signal" ou "V"
void sem_signal(SimulatedSemaphore* sem) {
    sem->value++;
    
    // Se há processos à espera, acorda um
    ProcessQueue* wq = &sem->waiting_queue;
    if (wq->count > 0) {
        // Remove o primeiro processo da fila de espera (FIFO)
        PCB* unblocked_process = wq->processes[wq->front];
        wq->front = (wq->front + 1) % MAX_PROCESSES_PER_QUEUE;
        wq->count--;

        // O processo agora pode tentar novamente obter o recurso.
        // Como o signal foi chamado, ele terá sucesso.
        if (sem_wait(sem, unblocked_process) == 0) {
             // Move o processo de volta para a fila de prontos do escalonador
            unblocked_process->state = PROCESS_READY;
            add_process_to_queue(unblocked_process);
            printf("-> Processo PID %d desbloqueado e movido para a fila de prontos.\n", unblocked_process->pid);
        }
    }
}