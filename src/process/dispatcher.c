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
        while (!is_process_finished(current_process)) {
            execute_process_instruction(current_process);
        }
        terminate_process(current_process); // usa a função centralizada
        current_process = NULL;
    } else {
        // Processos usuário executam por uma instrução
        execute_process_instruction(current_process);

        if (is_process_finished(current_process)) {
            terminate_process(current_process);
            current_process = NULL;
        } else if (current_process->quantum_remaining <= 0) {
            // Quantum esgotado, move para background
            demote_user_process(current_process);
            current_process = NULL;
        }
    }
}

// TODO: [Pessoa 1] Obter processo atual
PCB* get_current_process() {
    return current_process;
}