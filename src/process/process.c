#include "process.h"
#include "scheduler.h"
#include "../core/types.h"
#include "../../include/shared/constants.h"
#include "../memory/resources.h"
#include "../memory/memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../core/log.h"

// --- Variáveis Estáticas do Módulo ---
static int next_pid = 0;
static PCB* process_table[MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES];
static int process_count = 0;
static int loaded_pids[MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES];
static int loaded_count_global = 0;

// --- Implementação das Funções ---

void init_process() {
    for (int i = 0; i < MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES; i++) {
        process_table[i] = NULL;
    }
    process_count = 0;
    next_pid = 0;
    loaded_count_global = 0;
    printf("Módulo de processos inicializado.\n");
}

PCB* create_process(int priority, int cpu_time, int memory_blocks,
                    int start_time, int printer_code, int scanner_needed,
                    int modem_needed, int disk_code) {
    PCB* process = (PCB*)malloc(sizeof(PCB));
    if (!process) return NULL;

    process->pid = next_pid++;
    process->priority = (ProcessPriority)priority;
    process->state = PROCESS_READY;
    process->cpu_time = 0;
    process->total_cpu_time = cpu_time;
    process->memory_blocks = memory_blocks;
    process->memory_offset = -1; // Inicia como -1 para indicar que não foi alocado
    process->start_time = start_time;
    process->printer_code = printer_code;
    process->scanner_needed = scanner_needed;
    process->modem_needed = modem_needed;
    process->disk_code = disk_code;
    process->instruction_count = 0;
    process->quantum_remaining = (priority == 0) ? 0 : USER_PROCESS_QUANTUM;
    process->has_started = false;

    if (process_count < MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES) {
        process_table[process_count] = process;
        process_count++;
    }

    return process;
}

void print_process_info(PCB* process) {
    char col1[COL1_WIDTH], col2[COL2_WIDTH];
    
    print_section_header("DISPATCHER");
    
    snprintf(col1, COL1_WIDTH, "PID: %d", process->pid);
    snprintf(col2, COL2_WIDTH, "Offset: %d | Blocks: %d", 
             process->memory_offset, process->memory_blocks);
    print_log_entry(col1, col2);
    
    snprintf(col1, COL1_WIDTH, "Priority: %d", process->priority);
    snprintf(col2, COL2_WIDTH, "Time: %d/%d", 
             process->cpu_time, process->total_cpu_time);
    print_log_entry(col1, col2);
    
    snprintf(col1, COL1_WIDTH, "Resources");
    snprintf(col2, COL2_WIDTH, "P:%d S:%d M:%d D:%d",
             process->printer_code, process->scanner_needed,
             process->modem_needed, process->disk_code);
    print_log_entry(col1, col2);
    
    print_divider('-');
}

int load_processes_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro: não foi possível abrir o arquivo %s\n", filename);
        return -1;
    }

    char line[256];
    printf("Carregando processos do arquivo: %s\n", filename);

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '<' || line[0] == '\n' || line[0] == '#' || line[0] == '/') {
            continue;
        }

        int start_time, priority, cpu_time, memory_blocks;
        int printer_code, scanner_needed, modem_needed, disk_code;

        int parsed = sscanf(line, "%d, %d, %d, %d, %d, %d, %d, %d",
                           &start_time, &priority, &cpu_time, &memory_blocks,
                           &printer_code, &scanner_needed, &modem_needed, &disk_code);

        if (parsed != 8) continue;

        PCB* process = create_process(priority, cpu_time, memory_blocks,
                                    start_time, printer_code, scanner_needed,
                                    modem_needed, disk_code);
        if (!process) continue;

        int is_real_time = (process->priority == 0);
        int blocked = 0;

        if (process->memory_blocks > 0) {
            process->memory_offset = allocate_memory(process->memory_blocks, is_real_time);
            if (process->memory_offset == -1) {
                printf("-> FALHA CRÍTICA: Memória insuficiente para o processo PID %d. Processo descartado.\n", process->pid);
                destroy_process(process);
                continue;
            }
        }

        if (!is_real_time) {
            if (process->printer_code > 0 && !blocked) {
                if (allocate_io_resource(process, "printer")) {
                    printf("-> BLOQUEADO: Impressora indisponível para o processo PID %d.\n", process->pid);
                    blocked = 1;
                }
            }
            if (process->scanner_needed && !blocked) {
                if (allocate_io_resource(process, "scanner")) {
                    printf("-> BLOQUEADO: Scanner indisponível para o processo PID %d.\n", process->pid);
                    blocked = 1;
                }
            }
            if (process->modem_needed && !blocked) {
                if (allocate_io_resource(process, "modem")) {
                    printf("-> BLOQUEADO: Modem indisponível para o processo PID %d.\n", process->pid);
                    blocked = 1;
                }
            }
            if (process->disk_code > 0 && !blocked) {
                if (allocate_io_resource(process, "sata")) {
                    printf("-> BLOQUEADO: Disco SATA indisponível para o processo PID %d.\n", process->pid);
                    blocked = 1;
                }
            }
        }
        
        loaded_pids[loaded_count_global++] = process->pid;

        if (!blocked) {
            printf("-> SUCESSO: Recursos alocados para o processo PID %d.\n", process->pid);
            add_process_to_queue(process);
        } else {
             printf("-> Processo PID %d movido para a fila de espera do recurso.\n", process->pid);
        }
    }

    fclose(file);
    printf("Total de processos carregados (prontos ou bloqueados): %d\n\n", loaded_count_global);
    return loaded_count_global;
}

bool process_was_loaded(int pid) {
    for (int i = 0; i < loaded_count_global; i++) {
        if (loaded_pids[i] == pid) return true;
    }
    return false;
}

void destroy_process(PCB* process) {
    if (process == NULL) return;

    printf("-> Liberando recursos do processo PID %d...\n", process->pid);

    int is_real_time = (process->priority == 0);

    if (process->memory_offset != -1) {
        free_memory(process->memory_offset, process->memory_blocks);
    }

    if (!is_real_time) {
        if (process->scanner_needed) release_io_resource("scanner");
        if (process->printer_code > 0) release_io_resource("printer");
        if (process->modem_needed) release_io_resource("modem");
        if (process->disk_code > 0) release_io_resource("sata");
    }

    int found = 0;
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->pid == process->pid) {
            found = 1;
        }
        if (found && i + 1 < process_count) {
            process_table[i] = process_table[i + 1];
        }
    }

    if (found) {
        process_count--;
        process_table[process_count] = NULL;
    }

    printf("P%d return SIGINT\n", process->pid);
    free(process);
}

void execute_process_instruction(PCB* process) {
    if (!process || process->state != PROCESS_RUNNING) return;
    
    process->instruction_count++;
    process->cpu_time++;
    
    if (process->cpu_time >= process->total_cpu_time) {
        process->state = PROCESS_TERMINATED;
    }
    
    if (process->priority > 0) {
        process->quantum_remaining--;
    }
}

int is_process_finished(PCB* process) {
    return process && (process->cpu_time >= process->total_cpu_time);
}

int has_active_processes() {
    return process_count > 0;
}

void list_processes() {
    printf("=== LISTA DE PROCESSOS ===\n");
    printf("PID\tPrioridade\tEstado\t\tCPU Usado/Total\tMemória\n");
    printf("---\t----------\t------\t\t---------------\t-------\n");

    for (int i = 0; i < process_count; i++) {
        if (process_table[i]) {
            PCB* p = process_table[i];
            const char* state_str;

            switch (p->state) {
                case PROCESS_READY: state_str = "READY"; break;
                case PROCESS_RUNNING: state_str = "RUNNING"; break;
                case PROCESS_BLOCKED: state_str = "BLOCKED"; break;
                case PROCESS_TERMINATED: state_str = "TERMINATED"; break;
                default: state_str = "UNKNOWN"; break;
            }

            printf("%d\t%d\t\t%s\t\t%d/%d\t\t%d blocos\n",
                   p->pid, p->priority, state_str,
                   p->cpu_time, p->total_cpu_time, p->memory_blocks);
        }
    }
    printf("Total de processos: %d\n\n", process_count);
}

PCB* get_process_by_pid(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->pid == pid) {
            return process_table[i];
        }
    }
    return NULL;
}

int get_process_count() {
    return process_count;
}