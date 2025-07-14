#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

// Funções do escalonador
void init_scheduler();
PCB* schedule_real_time_process();
PCB* schedule_user_process();
int add_process_to_queue(PCB* process);
void demote_user_process(PCB* process);
void apply_aging();
int is_queue_empty(int priority);
void manage_background_processes(); // Gerencia processos em segundo plano

#endif // SCHEDULER_H