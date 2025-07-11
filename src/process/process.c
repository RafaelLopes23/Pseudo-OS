#include "process.h"
#include "scheduler.h"
#include "../core/types.h"
#include "../../include/shared/constants.h"
#include "../memory/resources.h"
#include "../memory/memory.h"
#include "../core/system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


static int next_pid = 0; // Contador global de PIDs
static PCB* process_table[MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES]; // Tabela global de processos
static int process_count = 0;

static int originally_loaded_pids[MAX_PROCESSES];
static int originally_loaded_count = 0;

static ProcessQueue waiting_queue;

void init_process() {
    // Inicializa a tabela de processos com NULL
    for (int i = 0; i < MAX_PROCESSES_PER_QUEUE * NUMBER_OF_PRIORITY_QUEUES; i++) {
        process_table[i] = NULL;
    }
    process_count = 0;
    next_pid = 0;

    waiting_queue.front = 0;
    waiting_queue.rear = -1;
    waiting_queue.count = 0;
    originally_loaded_count = 0;

    printf("Módulo de processos inicializado.\n");
}

// Função para criar um novo processo
PCB* create_process(int priority, int cpu_time, int memory_blocks,int start_time, int printer_code, int scanner_needed, int modem_needed, int disk_code) {
    
    PCB* process = (PCB*)malloc(sizeof(PCB));
    if (!process) return NULL;

    process->pid = next_pid++;
    process->priority = (ProcessPriority)priority;
    process->state = PROCESS_READY;
    process->cpu_time = 0;
    process->total_cpu_time = cpu_time;
    process->memory_blocks = memory_blocks;
    process->memory_offset = 0;                  // Responsabilidade do gerenciador de memória
    process->start_time = start_time;

    process->printer_code = printer_code;
    process->scanner_needed = scanner_needed;
    process->modem_needed = modem_needed;
    process->disk_code = disk_code;

    process->instruction_count = 0;
    process->quantum_remaining = (priority == 0) ? 0 : USER_PROCESS_QUANTUM;
    process->has_started = false;                   // inicializa flag do escalonamento

    // Coloca na tabela de processos
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
            // --- Validação de Pedidos Impossíveis ---
            bool impossible_request = false;

            //Verifica Memória
            if ((priority == 0 && memory_blocks > REAL_TIME_MEMORY_BLOCKS) ||
                (priority > 0 && memory_blocks > USER_MEMORY_BLOCKS)) {
                printf("-> FALHA IMPOSSÍVEL: PID %d solicita %d blocos de memória (excede o limite da região).\n", next_pid, memory_blocks);
                impossible_request = true;
            }

            //Verifica Recursos de E/S
            if (printer_code > NUMBER_OF_PRINTERS) {
                printf("-> FALHA IMPOSSÍVEL: PID %d solicita impressora %d, mas só existem %d.\n", next_pid, printer_code, NUMBER_OF_PRINTERS);
                impossible_request = true;
            }
            if (scanner_needed > NUMBER_OF_SCANNERS) {
                printf("-> FALHA IMPOSSÍVEL: PID %d solicita %d scanner(s), mas só existem %d.\n", next_pid, scanner_needed, NUMBER_OF_SCANNERS);
                impossible_request = true;
            }
            if (modem_needed > NUMBER_OF_MODEMS) {
                printf("-> FALHA IMPOSSÍVEL: PID %d solicita %d modem(s), mas só existem %d.\n", next_pid, modem_needed, NUMBER_OF_MODEMS);
                impossible_request = true;
            }
            if (disk_code > NUMBER_OF_SATA_DRIVES) {
                printf("-> FALHA IMPOSSÍVEL: PID %d solicita disco SATA %d, mas só existem %d.\n", next_pid, disk_code, NUMBER_OF_SATA_DRIVES);
                impossible_request = true;
            }
            
            //Se o pedido exeder os recursos totais do SO é descarta e vai para a próxima linha
            if (impossible_request) {
                next_pid++; // Garante que o próximo processo tenha um PID diferente
                continue;
            }
            
            //Se o pedido é válido cria o PCB
            PCB* process = create_process(priority, cpu_time, memory_blocks,
                                        start_time, printer_code, scanner_needed,
                                        modem_needed, disk_code);
            if (process) {
                //Adiciona o processo a FILA DE ESPERA para aguardar recursos
                waiting_queue.rear = (waiting_queue.rear + 1) % MAX_PROCESSES_PER_QUEUE;
                waiting_queue.processes[waiting_queue.rear] = process;
                waiting_queue.count++;
                processes_loaded++;
            }
        }
    }

    fclose(file);
    printf("Total de processos lidos para a fila de espera: %d\n\n", processes_loaded);
    return processes_loaded;

}

// Verifica se um PID foi carregado originalmente
bool process_was_loaded(int pid) {
    for (int i = 0; i < originally_loaded_count; i++) {
        if (originally_loaded_pids[i] == pid) {
            return true; // Encontrou
        }
    }
    return false; // Não achou
}

void terminate_process(PCB* process) {
    if (!process) 
        return;

    printf("Processo PID %d terminando a execução.\n", process->pid);
    process->state = PROCESS_TERMINATED;

    // Chama a função destroy_process para fazer toda a limpeza
    destroy_process(process);
}

void destroy_process(PCB* process) {
    if (process == NULL) {
        return;
    }

    printf("-> Liberando recursos do processo PID %d...\n", process->pid);

    //LIBERAR RECURSOS DE MEMoRIA E E/S
    int is_real_time = (process->priority == 0);
    if (process->memory_offset != -1) {
        free_memory(process->memory_offset, process->memory_blocks);
    }
    if (!is_real_time) {
        if (process->scanner_needed) 
            release_io_resource(&io_resources, "scanner");
        if (process->printer_code > 0) 
            release_io_resource(&io_resources, "printer");
        if (process->modem_needed) 
            release_io_resource(&io_resources, "modem");
        if (process->disk_code > 0) 
            release_io_resource(&io_resources, "sata");
    }

    process->state = PROCESS_TERMINATED;

    printf("P%d return SIGINT\n", process->pid);
}

void cleanup_process_table() {
    printf("Limpando a tabela de processos...\n");
    for (int i = 0; i < process_count; i++) {
        if (process_table[i]) {
            free(process_table[i]);
            process_table[i] = NULL;
        }
    }
    process_count = 0;
}


int is_waiting_queue_empty() {
    return waiting_queue.count == 0;
}


// Esta função verifica a fila de espera e tenta promover processos para a fila de prontos.
void check_and_promote_processes() {
    //para garantir justiça verificamos apenas o numero de processos que estavan na fila
    //evitando loops infinitos no mesmo ciclo.
    int processes_to_check = waiting_queue.count;
    if (processes_to_check == 0) {
        return; //nada a fazer
    }

    // Loop atraves da fila de espera
    for (int i = 0; i < processes_to_check; i++) {
        //Pega o primeiro processo(FIFO)
        PCB* process = waiting_queue.processes[waiting_queue.front];
        waiting_queue.front = (waiting_queue.front + 1) % MAX_PROCESSES_PER_QUEUE;
        waiting_queue.count--;

        //Verifica se os recursos para este processo estao disponiveis
        bool resources_ok = true;
        int is_real_time = (process->priority == PRIORITY_REAL_TIME);

        // Verifica a memória
        if (process->memory_blocks > 0) {
            if (!check_memory_availability(process->memory_blocks, is_real_time)) {
                resources_ok = false;
            }
        }

        // Verifica os recursos de E/S apenas se a memoria estiver OK e nao é de tempo real
        if (resources_ok && !is_real_time) {
            if (process->scanner_needed && !check_io_resource_availability(&io_resources, "scanner")) 
                resources_ok = false;
            if (resources_ok && process->printer_code > 0 && !check_io_resource_availability(&io_resources, "printer")) 
                resources_ok = false;
            if (resources_ok && process->modem_needed && !check_io_resource_availability(&io_resources, "modem")) 
                resources_ok = false;
            if (resources_ok && process->disk_code > 0 && !check_io_resource_availability(&io_resources, "sata")) 
                resources_ok = false;
        }

        if (resources_ok) {
            // Se os recursos estiverem disponiveis
            printf("-> Recursos disponíveis para PID %d. Alocando e promovendo para a fila de prontos.\n", process->pid);
            
            // Aloca a memória
            if (process->memory_blocks > 0) {
                process->memory_offset = allocate_memory(process->memory_blocks, is_real_time);
            }
            
            // Aloca os recursos de E/S
            if (!is_real_time) {
                if (process->scanner_needed) 
                    allocate_io_resource(&io_resources, "scanner");
                if (process->printer_code > 0) 
                    allocate_io_resource(&io_resources, "printer");
                if (process->modem_needed) 
                    allocate_io_resource(&io_resources, "modem");
                if (process->disk_code > 0) 
                    allocate_io_resource(&io_resources, "sata");
            }
            
            if (originally_loaded_count < MAX_PROCESSES) {
                originally_loaded_pids[originally_loaded_count++] = process->pid;
            }

            //Adiciona processo para as filas do escalonador
            add_process_to_queue(process);

        } else {
            //recursos ocupados o processo vai para o final da fila
            waiting_queue.rear = (waiting_queue.rear + 1) % MAX_PROCESSES_PER_QUEUE;
            waiting_queue.processes[waiting_queue.rear] = process;
            waiting_queue.count++;
        }
    }
}



// Execucao de instrução do processo
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

void list_processes() {
    printf("=== LISTA DE PROCESSOS ===\n");
    printf("PID\tPrioridade\tEstado\t\tCPU Usado/Total\tMemória\n");
    printf("---\t----------\t------\t\t---------------\t-------\n");

    for (int i = 0; i < process_count; i++) {
        if (process_table[i]) {
            PCB* p = process_table[i];
            const char* state_str;

            switch (p->state) {
                case PROCESS_READY: 
                    state_str = "READY"; 
                    break;
                case PROCESS_RUNNING: 
                    state_str = "RUNNING";
                    break;
                case PROCESS_BLOCKED:
                    state_str = "BLOCKED";
                    break;
                case PROCESS_TERMINATED: 
                    state_str = "TERMINATED"; 
                    break;
                default: 
                    state_str = "UNKNOWN"; 
                    break;
            }

            printf("%d\t%d\t\t%s\t\t%d/%d\t\t%d blocos\n", p->pid, p->priority, state_str, p->cpu_time, p->total_cpu_time, p->memory_blocks);
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

int has_active_processes() {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i] && process_table[i]->state != PROCESS_TERMINATED) {
            return 1; // Tem processo ativo
        }
    }
    return 0; // Nao tem processo ativo
}