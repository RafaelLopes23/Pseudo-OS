#include <stdio.h>
#include "process.h"
#include "scheduler.h"

// TODO: [Pessoa 1] Implementar testes para gerenciamento de processos

void test_process_creation() {
    // TODO: Implementar teste para criação de processo
    // Exemplo: Criar um processo e verificar se o PID é válido
}

void test_process_scheduling() {
    // TODO: Implementar teste para escalonamento de processos
    // Exemplo: Adicionar processos e verificar a ordem de execução
}

void test_process_dispatching() {
    // TODO: Implementar teste para o dispatcher
    // Exemplo: Verificar se o dispatcher está chamando os processos corretamente
}

int main() {
    // Executar todos os testes
    test_process_creation();
    test_process_scheduling();
    test_process_dispatching();

    printf("Todos os testes de processo foram executados.\n");
    return 0;
}