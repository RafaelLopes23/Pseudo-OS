#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Incluindo os cabeçalhos de todos os módulos do sistema
#include "../filesystem/filesystem.h"
#include "../filesystem/fileops.h"
#include "../memory/memory.h"
#include "../memory/resources.h"
#include "../process/process.h"
#include "../process/dispatcher.h"
#include "../process/scheduler.h"
#include <ctype.h>

static int processes_loaded_global = 0;  //total de processos carregados

int debug_mode = 0;


// --- Variáveis Globais do Sistema ---
extern MemoryManager mem_manager;
extern IOResources io_resources;


// --- Funções do Núcleo do Sistema ---


void trim_whitespace(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    *(end + 1) = '\0';

    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

void initialize_system() {
    printf("Inicializando o pseudo-SO...\n");
    initialize_memory(&mem_manager);
    init_io_resources(&io_resources);
    init_process();
    init_scheduler();
    printf("Sistema inicializado com sucesso.\n\n");
}

void shutdown_system() {
    printf("\nDesligando o pseudo-SO...\n");
    cleanup_process_table(); // <-- ADICIONAR
    destroy_memory_manager();
    destroy_io_resources();
    printf("Sistema desligado.\n");
}

void start_main_loop() {
    printf("--- Iniciando Loop Principal de Execução ---\n");

    while (has_active_processes() || !is_waiting_queue_empty()) {
        
        // O dispatcher tenta obter um processo PRONTO para executar
        PCB* current_process = dispatch_process();

        if (current_process) {
            // Se encontrou um processo pronto, executa uma fatia de tempo
            run_current_process();
        } else {
            // Se não, a CPU está "ociosa".
            // Tenta promover processos da fila de espera para a de prontos
            check_and_promote_processes();

            // Se após a tentativa de promoção não há ninguém pronto
            if (!has_active_processes()) {
                printf("Sistema ocioso. Aguardando liberação de recursos...\n");
                sleep(1); // Dorme por 1 segundo para não gastar CPU desnecessariamente
            }

            // Aplica o envelhecimento (aging) para os processos que estão nas filas de usuário
            apply_aging();
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

    // --- Leitura do Cabeçalho ---
    if (!fgets(line, sizeof(line), file)) { 
        fclose(file); 
        return; 
    }
    sscanf(line, "%d", &total_disk_blocks);
    init_filesystem(total_disk_blocks);

    if (!fgets(line, sizeof(line), file)) { 
        fclose(file); 
        return; 
    }
    sscanf(line, "%d", &num_initial_files);
    if (debug_mode) printf("\nDEBUG: Disco inicializado com %d blocos. Esperando %d arquivos iniciais.\n", total_disk_blocks, num_initial_files);

    // --- Leitura do Estado Inicial ---
    printf("Carregando estado inicial do disco...\n");
    for (int i = 0; i < num_initial_files; i++) {
        if (!fgets(line, sizeof(line), file)) 
            break;
        if (debug_mode) 
            printf("DEBUG-INIT: Linha lida: [%s]", line);
        line[strcspn(line, "\r\n")] = 0;

        char name[MAX_FILENAME_LEN] = {0};
        uint32_t first_block = 0, num_blocks = 0;

        if (sscanf(line, "%[^,],%u,%u", name, &first_block, &num_blocks) == 3) {
            if (debug_mode) 
                printf("DEBUG-INIT: Parseado -> Nome: '[%s]', Bloco inicial: %u, Tamanho: %u\n", name, first_block, num_blocks);
            if (create_initial_file(name, first_block, num_blocks) != 0) {
                fprintf(stderr, "Erro ao criar arquivo inicial '%s'\n", name);
            }
        } else {
            if (debug_mode) 
                printf("DEBUG-INIT: Falha no sscanf da linha inicial.\n");
        }
    }
    printf("Estado inicial do disco carregado.\n");


    // --- Leitura e Processamento das Operações ---
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '\n' || line[0] == '#') 
            continue;
        if (debug_mode) 
            printf("\nDEBUG-OP: Linha lida: [%s]", line);
        line[strcspn(line, "\r\n")] = 0;

        int process_id = -1, op_code = -1, num_blocks = -1;
        char file_name[MAX_FILENAME_LEN] = {0};

        sscanf(line, "%d,%d,", &process_id, &op_code);
        if (debug_mode) 
            printf("DEBUG-OP: Parse inicial -> PID: %d, OpCode: %d\n", process_id, op_code);

        if (process_id == -1 || op_code == -1) {
            if (debug_mode) 
                printf("DEBUG-OP: Falha ao parsear PID ou OpCode. Pulando linha.\n");
            continue;
        }

        if (!process_was_loaded(process_id)) {
            printf("Operação Ignorada: Processo com PID %d não foi carregado.\n", process_id);
            continue;
        }

        if (op_code == 0) {
            if (sscanf(line, "%*d,%*d, %254[^,],%d", file_name, &num_blocks) == 2) {
                if (debug_mode) 
                    printf("DEBUG-OP: Tentando CRIAR. Nome: '[%s]', Blocos: %d\n", file_name, num_blocks);
                if (create_file(file_name, num_blocks, process_id) == 0) {
                    printf("Operação Sucesso: Processo %d criou o arquivo %s.\n", process_id, file_name);
                } else {
                    printf("Operação Falha: Processo %d não pôde criar o arquivo %s.\n", process_id, file_name);
                }
            }
        } else if (op_code == 1) {
            if (sscanf(line, "%*d,%*d,%254s", file_name) == 1) {
                //Busca o PCB do processo para obter sua prioridade
                PCB* p = get_process_by_pid(process_id);
                //Verifica se o processo foi encontrado
                if (p) {
                    if (delete_file(file_name, process_id, p->priority) == 0) {
                        printf("Operação Sucesso: Processo %d deletou o arquivo %s.\n", process_id, file_name);
                    } else {
                        printf("Operação Falha: Processo %d não pôde deletar o arquivo %s.\n", process_id, file_name);
                    }
                }
            }
        }
    }
    fclose(file);
}

void print_disk_map() {
    printf("\n--- Estado Final do Disco ---\n");
    list_files();
    check_disk_usage();
    print_disk_block_map();
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_de_processos> <arquivo_de_filesystem>\n", argv[0]);
        return 1;
    }
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

    // Inicializar Módulos
    initialize_system();

    // Carregar Processos
    int num_processes = load_processes_from_file(processes_file);
    if (num_processes <= 0) {
        fprintf(stderr, "Nenhum processo foi carregado. Encerrando.\n");
        shutdown_system();
        return 1;
    }
    processes_loaded_global = num_processes;

    // Iniciar Loop de Execução
    start_main_loop();

    // Processar Operações de Arquivo
    printf("\n--- Processando Operações de Arquivo ---\n");
    process_filesystem_operations(filesystem_file);

    // Imprimir Estado Final do Disco
    print_disk_map();

    // Desligar Sistema
    shutdown_system();

    return 0;
}