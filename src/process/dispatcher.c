#include "dispatcher.h"
#include "scheduler.h"
#include "process.h"
#include "../../include/shared/constants.h"  // ADICIONAR ESTA LINHA
#include <stdio.h>
#include <stdlib.h>

static PCB* current_process = NULL;

// TODO: [Pessoa 1] Implementar dispatcher principal
PCB* dispatch_process() {
    // Primeiro, verifica processos de tempo real (maior prioridade)
    PCB* process = schedule_real_time_process();

    // Se não há processos tempo real, verifica processos de usuário
    if (!process) {
        process = schedule_user_process();
    }

    if (process) {
        current_process = process;
        if (!process->has_started) {
            print_process_info(process);
            printf("process %d =>\n", process->pid);
            printf("P%d STARTED\n", process->pid);
            process->has_started = true;
        }
    }

    return process;
}

// TODO: [Pessoa 1] Executar processo atual
void run_current_process() {
    if (!current_process) return;

    if (current_process->priority == PRIORITY_REAL_TIME) {
        // Processos tempo real executam até terminar
    
        // Processos tempo real executam uma instrução por chamada
        execute_process_instruction(current_process);

        // Se o processo terminou após a instrução, ele é destruído.
        if (is_process_finished(current_process)) {
            destroy_process(current_process);
            current_process = NULL;
        } else {
            // Se não terminou, ele é colocado de volta no início da fila para
            // garantir que será o próximo a ser executado (FIFO).
            add_process_to_queue(current_process);
            current_process = NULL;
        }

        // Printar só destruição via return SIGINT dentro de destroy_process
    } else {
        // Processos usuário executam por quantum
        execute_process_instruction(current_process);

        if (is_process_finished(current_process)) {
            destroy_process(current_process);
            current_process = NULL;
            // demote ou liberação será logado dentro de destroy_process
        } else if (current_process->quantum_remaining <= 0) {
            demote_user_process(current_process);
            current_process = NULL;
        }
    }
}

// TODO: [Pessoa 1] Obter processo atual
PCB* get_current_process() {
    return current_process;
}