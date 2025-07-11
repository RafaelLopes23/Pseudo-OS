#include "dispatcher.h"
#include "scheduler.h"
#include "process.h"
#include "../../include/shared/constants.h"
#include <stdio.h>
#include <stdlib.h>

static PCB* current_process = NULL;

PCB* dispatch_process() {
    // Primeiro, verifica processos de tempo real
    PCB* process = schedule_real_time_process();

    // Se não tem verifica processos de usuário
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

void run_current_process() {
    if (!current_process) return;

    if (current_process->priority == PRIORITY_REAL_TIME) {

        //Processos tempo real executam ate terminar
        //Processos tempo real executam uma instrução por chamada
        execute_process_instruction(current_process);

        //Se o processo terminou ele é destruído
        if (is_process_finished(current_process)) {
            destroy_process(current_process);
            current_process = NULL;
        } else {
            // Se nao terminou volta no início da fila para
            // garantir que sera o próximo pela politica FIFO
            add_process_to_queue(current_process);
            current_process = NULL;
        }
    } else {
        // Processos usuário executam por quantum
        execute_process_instruction(current_process);

        if (is_process_finished(current_process)) {
            destroy_process(current_process);
            current_process = NULL;
        } else if (current_process->quantum_remaining <= 0) {
            demote_user_process(current_process);
            current_process = NULL;
        }
    }
}

PCB* get_current_process() {
    return current_process;
}