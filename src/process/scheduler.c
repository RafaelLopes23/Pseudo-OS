// src/process/scheduler.c
#include "scheduler.h"

// TODO: [Pessoa 1] Implementar algoritmo FIFO para processos tempo real
int schedule_real_time_process() {
    // Implementar aqui: buscar próximo processo na fila tempo real
    // Retornar PID do processo ou -1 se fila vazia
    return -1;  // placeholder
}

// TODO: [Pessoa 1] Implementar algoritmo de escalonamento para processos de usuário
int schedule_user_process() {
    // Implementar aqui: buscar próximo processo na fila de usuário
    // Retornar PID do processo ou -1 se fila vazia
    return -1;  // placeholder
}

// TODO: [Pessoa 1] Implementar função para adicionar processo à fila
void add_process_to_queue(int pid, int priority) {
    // Implementar aqui: adicionar processo à fila correspondente
}

// TODO: [Pessoa 1] Implementar função para remover processo da fila
void remove_process_from_queue(int pid) {
    // Implementar aqui: remover processo da fila correspondente
}