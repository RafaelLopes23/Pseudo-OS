#ifndef SYSTEM_H
#define SYSTEM_H

#include "types.h"

// Constantes do sistema
#define MAX_PROCESSES 1000
#define MAX_MEMORY_BLOCKS 1024
#define IO_EVENT_BUFFER_SIZE 64

// Funções principais do sistema
void initialize_system();
void start_main_loop();
void shutdown_system();

#endif // SYSTEM_H