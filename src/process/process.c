#include "process.h"
#include "../core/types.h"
#include "../../include/shared/constants.h"
#include <stdlib.h>
#include <stdio.h>

static int next_pid = 0; // Contador global de PIDs
static PCB* process_table[MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES]; // Tabela global de processos
static int process_count = 0;

// TODO: [Pessoa 1] Implementar inicialização do módulo de processos
void init_process() {
    // Inicializa a tabela de processos
    for (int i = 0; i < MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES; i++) {
        process_table[i] = NULL;
    }
    process_count = 0;
    next_pid = 0;
    printf("Módulo de processos inicializado.\n");
}

// Função para criar um novo processo
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
    process->memory_offset = 0; // Será definido pelo gerenciador de memória
    process->start_time = start_time;

    process->printer_code = printer_code;
    process->scanner_needed = scanner_needed;
    process->modem_needed = modem_needed;
    process->disk_code = disk_code;
    
    process->instruction_count = 0;
    process->quantum_remaining = (priority == 0) ? 0 : USER_PROCESS_QUANTUM;

    // Adiciona à tabela de processos
    if (process_count < MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES) {
        process_table[process_count] = process;
        process_count++;
    }

    return process;
}

// Print informações do processo
void print_process_info(PCB* process) {
    if (!process) return;

    printf("dispatcher =>\n");
    printf("PID: %d\n", process->pid);
    printf("offset: %d\n", process->memory_offset);
    printf("blocks: %d\n", process->memory_blocks);
    printf("priority: %d\n", process->priority);
    printf("time: %d\n", process->total_cpu_time);
    printf("printers: %d\n", process->printer_code);
    printf("scanners: %d\n", process->scanner_needed);
    printf("modems: %d\n", process->modem_needed);
    printf("drives: %d\n", process->disk_code);
}

int load_processes_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro: não foi possível abrir o arquivo %s\n", filename);
        return -1;
    }
    
    char line[256];
    int processes_loaded = 0;
    
    printf("Carregando processos do arquivo: %s\n", filename);
    
    while (fgets(line, sizeof(line), file)) {
        // Pular linhas de comentário ou vazias
        if (line[0] == '<' || line[0] == '\n' || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        int start_time, priority, cpu_time, memory_blocks;
        int printer_code, scanner_needed, modem_needed, disk_code;
        
        // Parse da linha: tempo_inicialização, prioridade, tempo_processador, blocos_memoria, impressora, scanner, modem, disco
        int parsed = sscanf(line, "%d, %d, %d, %d, %d, %d, %d, %d",
                           &start_time, &priority, &cpu_time, &memory_blocks,
                           &printer_code, &scanner_needed, &modem_needed, &disk_code);
        
        if (parsed == 8) {
            // Criar o processo
            PCB* process = create_process(priority, cpu_time, memory_blocks,
                                        start_time, printer_code, scanner_needed,
                                        modem_needed, disk_code);
            
            if (process) {
                printf("Processo criado: PID=%d, Prioridade=%d, CPU=%d, Memória=%d\n", 
                       process->pid, process->priority, process->total_cpu_time, process->memory_blocks);
                
                // TODO: [Pessoa 2] Integração com gerenciador de memória
                // TODO: [Pessoa 2] Verificar recursos E/S disponíveis
                
                // Por enquanto, adiciona direto à fila (integração será feita depois)
                processes_loaded++;
            } else {
                printf("Erro: não foi possível criar processo\n");
            }
        } else {
            printf("Aviso: linha ignorada (formato inválido): %s", line);
        }
    }
    
    fclose(file);
    printf("Total de processos carregados: %d\n\n", processes_loaded);
    return processes_loaded;
}

// TODO: [Pessoa 1] Implementar término de processo
void terminate_process(PCB* process) {
    if (!process) return;
    
    process->state = PROCESS_TERMINATED;
    printf("P%d terminando...\n", process->pid);
    
    // Remove da tabela de processos
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->pid == process->pid) {
            // Move o último processo para esta posição
            process_table[i] = process_table[process_count - 1];
            process_table[process_count - 1] = NULL;
            process_count--;
            break;
        }
    }
}

// Destruição de processo
void destroy_process(PCB* process) {
    if (process) {
        printf("P%d return SIGINT\n", process->pid);
        free(process);
    }
}

// Execução de instrução do processo
void execute_process_instruction(PCB* process) {
    if (!process || process->state != PROCESS_RUNNING) return;

    process->instruction_count++;
    process->cpu_time++;

    printf("P%d instruction %d\n", process->pid, process->instruction_count);

    // Verifica se o processo terminou
    if (process->cpu_time >= process->total_cpu_time) { 
        process->state = PROCESS_TERMINATED;
    }

    // Decrementa quantum para processos de usuário
    if (process->priority > 0) {
        process->quantum_remaining--;
    }
}

// Verificar se processo terminou
int is_process_finished(PCB* process) {
    return process && (process->cpu_time >= process->total_cpu_time);
}

// TODO: [Pessoa 1] Implementar listagem de processos
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

// TODO: [Pessoa 1] Implementar busca de processo pelo PID
PCB* get_process_by_pid(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->pid == pid) {
            return process_table[i];
        }
    }
    return NULL; // Processo não encontrado
}

// TODO: [Pessoa 1] Função auxiliar para obter contador de processos
int get_process_count() {
    return process_count;
}

// TODO: [Pessoa 1] Função para verificar se há processos ativos
int has_active_processes() {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->state != PROCESS_TERMINATED) {
            return 1; // Há pelo menos um processo ativo
        }
    }
    return 0; // Nenhum processo ativo
}