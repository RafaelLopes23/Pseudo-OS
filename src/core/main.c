#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "../filesystem/filesystem.h"
#include "../filesystem/fileops.h"
#include "../memory/memory.h"
#include "../memory/resources.h"
#include "../process/process.h"
#include "../process/dispatcher.h"
#include "../process/scheduler.h"
#include "semaphore.h"
#include "log.h"

// ... (variáveis globais) ...
extern MemoryManager mem_manager;
extern IOResources io_resources;
extern APFS fs;

// --- Funções do Núcleo do Sistema ---

void initialize_system() {
    printf("Inicializando o pseudo-SO...\n");
    initialize_memory(&mem_manager);
    init_io_resources(&io_resources);
    init_process();
    init_scheduler();
    init_filesystem();
    printf("Sistema inicializado com sucesso.\n\n");
}

void shutdown_system() {
    printf("\nDesligando o pseudo-SO...\n");
    printf("Sistema desligado.\n");
}

void start_main_loop() {
    printf("--- Iniciando Loop Principal de Execução ---\n");
    while (has_active_processes()) {
        PCB* current_process = dispatch_process();
        if (current_process) {
            run_current_process();
        } else {
            // Se não há processos prontos, o sistema apenas espera
            // para não sobrecarregar a CPU com um loop vazio.
            sleep(1); 
        }
        // A gestão de processos em background  é chamada a cada ciclo
        // do loop, independentemente de a CPU estar ocupada ou não.
        manage_background_processes();
    }
    printf("--- Loop Principal de Execução Terminado ---\n\n");
}

void process_filesystem_operations(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo de sistema de arquivos %s\n", filename);
        return;
    }
    
    char line[256];
    int total_disk_blocks, num_initial_files;
    fgets(line, sizeof(line), file);
    sscanf(line, "%d", &total_disk_blocks);
    fgets(line, sizeof(line), file);
    sscanf(line, "%d", &num_initial_files);
    for (int i = 0; i < num_initial_files; i++) {
        char name[MAX_FILENAME_LEN];
        int first_block, num_blocks;
        fgets(line, sizeof(line), file);
        sscanf(line, "%[^,], %d, %d", name, &first_block, &num_blocks);
        if (create_initial_file(name, first_block, num_blocks) != 0) {
            fprintf(stderr, "Erro: não foi possível criar arquivo inicial %s\n", name);
        }
    }

    while (fgets(line, sizeof(line), file)) {
        int process_id, op_code, num_blocks = 0;
        char file_name[MAX_FILENAME_LEN] = {0};
        
        line[strcspn(line, "\r\n")] = 0;
        
        int parsed = sscanf(line, "%d,%d, %[^,],%d", &process_id, &op_code, file_name, &num_blocks);
        if (parsed < 2) continue;
        if (strlen(file_name) == 0 && op_code != 1) strcpy(file_name, "N/A");


        if (!process_was_loaded(process_id)) {
            log_operation_result(process_id, "N/A", file_name, 2, "PID não foi carregado inicialmente");
            continue;
        }

        if (op_code == 0) { // Criar
             if (parsed < 4) {
                log_operation_result(process_id, "criar", file_name, 0, "Argumentos insuficientes");
                continue;
            }
            if (create_file(file_name, num_blocks, process_id) == 0) {
                char details[50];
                snprintf(details, sizeof(details), "%d blocos", num_blocks);
                log_operation_result(process_id, "criar", file_name, 1, details);
            }
        } else if (op_code == 1) { // Deletar
             if (parsed < 3) {
                log_operation_result(process_id, "remover", file_name, 0, "Argumentos insuficientes");
                continue;
            }
            delete_file(file_name, process_id);
        }
    }

    fclose(file);
}

void print_disk_map() {
    printf("\n--- Estado Final do Disco ---\n");
    list_files();
    check_disk_usage();
    // Imprime o mapa de ocupação por bloco após o resumo
    print_disk_block_map();
}


// --- Ponto de Entrada Principal do Programa ---

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_de_processos> <arquivo_de_filesystem>\n", argv[0]);
        return 1;
    }

    const char* processes_file = argv[1];
    const char* filesystem_file = argv[2];

    // Redireciona stdout e stderr para log.txt
    freopen("log.txt", "w", stdout);
    freopen("log.txt", "a", stderr);

    initialize_system();
    load_processes_from_file(processes_file);
    start_main_loop();

    printf("\n--- Processando Operações de Arquivo ---\n");
    process_filesystem_operations(filesystem_file);
    
    print_disk_map();
    shutdown_system();

    log_simulation_outcome(true); //registar o sucesso da simulação

    return 0;
}