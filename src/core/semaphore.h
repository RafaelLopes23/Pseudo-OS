#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "../process/process.h"

// Estrutura para um semáforo simulado
typedef struct {
    int value;                // Valor do semáforo (nº de recursos disponíveis)
    ProcessQueue waiting_queue; // Fila de processos à espera deste semáforo
} SimulatedSemaphore;

// Funções do semáforo
void init_semaphore(SimulatedSemaphore* sem, int initial_value);
int sem_wait(SimulatedSemaphore* sem, PCB* process);
void sem_signal(SimulatedSemaphore* sem);

#endif // SEMAPHORE_H