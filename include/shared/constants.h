#ifndef CONSTANTS_H
#define CONSTANTS_H

// Definido as constantes para o sistema operacional

// Número máximo de processos por fila
#define MAX_PROCESSES_PER_QUEUE 1000

// Número total de blocos de memória
#define TOTAL_MEMORY_BLOCKS 1024

// Número de blocos reservados para processos de tempo real
#define REAL_TIME_MEMORY_BLOCKS 64

// Número de blocos compartilhados para processos de usuário
#define USER_MEMORY_BLOCKS (TOTAL_MEMORY_BLOCKS - REAL_TIME_MEMORY_BLOCKS)

// Quantum de tempo para processos de usuário (em milissegundos)
#define USER_PROCESS_QUANTUM 1

// Número de filas de prioridade
#define NUMBER_OF_PRIORITY_QUEUES 4

// Número de dispositivos E/S
#define NUMBER_OF_SCANNERS 1
#define NUMBER_OF_PRINTERS 2
#define NUMBER_OF_MODEMS 1
#define NUMBER_OF_SATA_DRIVES 2
// Número de "ticks" que um processo pode ficar em background antes de ser suspenso
#define BACKGROUND_TIME_LIMIT 5

#endif // CONSTANTS_H