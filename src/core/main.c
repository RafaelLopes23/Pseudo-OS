#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para a função sleep

// Incluindo os cabeçalhos de todos os módulos do sistema
#include "../filesystem/filesystem.h"
#include "../filesystem/fileops.h"
#include "../memory/memory.h"
#include "../memory/resources.h"
#include "../process/process.h"
#include "../process/dispatcher.h"
#include "../process/scheduler.h"

static int processes_loaded_global = 0;  // total de processos carregados


// --- Variáveis Globais do Sistema ---
// Definidas em seus respectivos módulos e declaradas como 'extern' nos headers
extern MemoryManager mem_manager;
extern IOResources io_resources;
extern APFS fs;  // Sistema de arquivos global


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
            sleep(4);
            apply_aging();  // Aplica aging para evitar starvation quando sem processos prontos
        }
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
        // Cria arquivo inicial na posição especificada
        if (create_initial_file(name, first_block, num_blocks) != 0) {
            fprintf(stderr, "Erro: não foi possível criar arquivo inicial %s (blocos %d-%d)\n",
                    name, first_block, first_block + num_blocks - 1);
        }
    }

    while (fgets(line, sizeof(line), file)) {
        int process_id, op_code, num_blocks;
        char file_name[MAX_FILENAME_LEN];
        int parsed = sscanf(line, "%d, %d, %[^,], %d", &process_id, &op_code, file_name, &num_blocks);
        if (parsed < 3) continue;
        // Verifica se PID foi carregado originalmente
        if (!process_was_loaded(process_id)) {
            printf("Operação Ignorada: Processo com PID %d não faz parte do conjunto inicial.\n", process_id);
            continue;
        }
        // Verifica se o nome do arquivo é válido
        if (op_code == 0 && parsed == 4) {
            if (create_file(file_name, num_blocks, process_id) == 0) {
                printf("Operação Sucesso: PID %d criou '%s'\n", process_id, file_name);
            }
        } else if (op_code == 1) {
            int result = delete_file(file_name, process_id);
            if (result == 0) {
                printf("Operação Sucesso: PID %d deletou '%s'\n", process_id, file_name);
            } else if (result == -2) {
                printf("Operação Falha: PID %d não tem permissão para deletar '%s'\n", process_id, file_name);
            }
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


void clear_log_file() {
    FILE* log_file = fopen("log.txt", "w");
    if (log_file) {
        fclose(log_file);
        printf("Arquivo de log limpo.\n");
    }
    else {
        fprintf(stderr, "Erro ao limpar o arquivo de log.\n");
    }
}
// --- Ponto de Entrada Principal do Programa ---

int main(int argc, char *argv[]) {
    // 1. Validar Argumentos
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_de_processos> <arquivo_de_filesystem>\n", argv[0]);
        return 1;
    }
    clear_log_file(); //limpa o arquivo de log antes de iniciar

    printf("Iniciando o Pseudo-OS com os arquivos:\n");

    const char* processes_file = argv[1];
    const char* filesystem_file = argv[2];

    // Abrir arquivo de log
    if (!freopen("log.txt", "w", stdout)) {
        perror("Erro ao criar log.txt");
        return 1;
    }
    if (!freopen("log.txt", "a", stderr)) {
        perror("Erro ao redirecionar stderr para log.txt");
        return 1;
    }

    // 2. Inicializar Módulos
    initialize_system();

    // 3. Carregar Processos
    int num_processes = load_processes_from_file(processes_file);
    if (num_processes <= 0) {
        fprintf(stderr, "Nenhum processo foi carregado. Encerrando.\n");
        shutdown_system();
        return 1;
    }
    processes_loaded_global = num_processes;

    // 4. Iniciar Loop de Execução
    start_main_loop();

    // 5. Processar Operações de Arquivo
    printf("\n--- Processando Operações de Arquivo ---\n");
    process_filesystem_operations(filesystem_file);

    // 6. Imprimir Estado Final do Disco
    print_disk_map();

    // 7. Desligar Sistema
    shutdown_system();

    return 0;
}