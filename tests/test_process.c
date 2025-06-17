#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/process/process.h"
#include "../src/process/scheduler.h"
#include "../src/process/dispatcher.h"

void test_process_creation() {
    printf("=== Teste: Criação de Processo ===\n");
    
    // Inicializar módulo de processos
    init_process();
    
    // Criar um processo de tempo real
    PCB* process1 = create_process(0, 3, 64, 0, 0, 0, 0, 0);
    assert(process1 != NULL);
    assert(process1->pid == 0);
    assert(process1->priority == PRIORITY_REAL_TIME);
    assert(process1->total_cpu_time == 3);
    assert(process1->memory_blocks == 64);
    
    // Criar um processo de usuário
    PCB* process2 = create_process(1, 5, 32, 1, 1, 0, 0, 0);
    assert(process2 != NULL);
    assert(process2->pid == 1);
    assert(process2->priority == PRIORITY_USER_1);
    assert(process2->total_cpu_time == 5);
    
    printf("✓ Criação de processos funcionando corretamente\n");
    printf("✓ Processo 1: PID=%d, Prioridade=%d\n", process1->pid, process1->priority);
    printf("✓ Processo 2: PID=%d, Prioridade=%d\n", process2->pid, process2->priority);
    
    destroy_process(process1);
    destroy_process(process2);
}

void test_process_scheduling() {
    printf("\n=== Teste: Escalonamento de Processos ===\n");
    
    // Inicializar escalonador
    init_scheduler();
    
    // Criar processos de diferentes prioridades
    PCB* rt_process = create_process(0, 2, 64, 0, 0, 0, 0, 0);
    PCB* user_process1 = create_process(1, 3, 32, 1, 0, 0, 0, 0);
    PCB* user_process2 = create_process(2, 4, 16, 2, 0, 0, 0, 0);
    
    // Adicionar à filas
    assert(add_process_to_queue(rt_process) == 0);
    assert(add_process_to_queue(user_process1) == 0);
    assert(add_process_to_queue(user_process2) == 0);
    
    // Processos tempo real devem ter prioridade
    PCB* scheduled = schedule_real_time_process();
    assert(scheduled != NULL);
    assert(scheduled->pid == rt_process->pid);
    printf("✓ Processo tempo real escalonado corretamente: PID=%d\n", scheduled->pid);
    
    // Processos usuário devem seguir ordem de prioridade
    PCB* user_scheduled = schedule_user_process();
    assert(user_scheduled != NULL);
    assert(user_scheduled->pid == user_process1->pid);
    printf("✓ Processo usuário escalonado corretamente: PID=%d\n", user_scheduled->pid);
    
    destroy_process(rt_process);
    destroy_process(user_process1);
    destroy_process(user_process2);
}

void test_process_execution() {
    printf("\n=== Teste: Execução de Processo ===\n");
    
    PCB* process = create_process(1, 3, 32, 0, 0, 0, 0, 0);
    process->state = PROCESS_RUNNING;
    
    printf("Executando instruções do processo:\n");
    
    // Executar algumas instruções
    for (int i = 0; i < 3; i++) {
        execute_process_instruction(process);
        if (is_process_finished(process)) {
            printf("✓ Processo terminou após %d instruções\n", process->instruction_count);
            break;
        }
    }
    
    assert(is_process_finished(process));
    destroy_process(process);
}

void test_load_processes_from_file() {
    printf("\n=== Teste: Carregamento de Arquivo ===\n");
    
    init_process();
    
    // Testar carregamento do arquivo de exemplo
    int loaded = load_processes_from_file("examples/processes.txt");
    
    if (loaded > 0) {
        printf("✓ Arquivo carregado com sucesso: %d processos\n", loaded);
        
        // Listar processos carregados
        list_processes();
    } else {
        printf("⚠ Não foi possível carregar o arquivo (normal se não existir)\n");
    }
}

int main() {
    printf("===== TESTES DO MÓDULO DE PROCESSOS =====\n");
    
    test_process_creation();
    test_process_scheduling();
    test_process_execution();
    test_load_processes_from_file();
    
    printf("\n===== TODOS OS TESTES CONCLUÍDOS =====\n");
    return 0;
}