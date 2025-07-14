#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include "../core/types.h"

// Funções relacionadas ao gerenciamento de processos
void init_process();
PCB* create_process(int priority, int cpu_time, int memory_blocks,
        int start_time, int printer_code, int scanner_needed,
        int modem_needed, int disk_code);
void print_process_info(PCB* process);
void terminate_process(PCB* process);
void destroy_process(PCB* process);
void execute_process_instruction(PCB* process);
int is_process_finished(PCB* process);
void list_processes();
PCB* get_process_by_pid(int pid);
int get_process_count();
int has_active_processes();
int load_processes_from_file(const char* filename);
bool process_was_loaded(int pid); // para verificar se um PID foi carregado
void release_all_resources(PCB* process); //libertar recursos de um processo

#endif // PROCESS_H