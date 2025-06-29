#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "types.h"
#include "process.h"      // Novos includes
#include "dispatcher.h"   // Novos includes

// Variável de estado
static int system_initialized = 0;

// Implementação completa
void initialize_system() {
    if (system_initialized) return;
    
    init_filesystem();      // Integração com filesystem
    init_memory_manager();  // Integração com memory (Pessoa 2)
    init_process();         // Integração com process (Pessoa 1)
    init_scheduler();       // Integração com scheduler
    
    system_initialized = 1;
    printf("Sistema inicializado\n");
}

// Loop principal completo
void start_main_loop() {
    while (1) {
        PCB *process = dispatch_process();  // Usa dispatcher
        if (process) run_current_process();
        usleep(100000);  // Controle de CPU
    }
}

int main() {
    initialize_system();
    start_main_loop();
    return 0;
}

/*
#include <stdio.h>
#include "system.h"
#include "types.h"

// TODO: [Pessoa 3] Implementar a inicialização do sistema
// TODO: [Pessoa 3] Implementar o loop principal do sistema

int main() {
    // Inicializar o sistema
    initialize_system();

    // Loop principal do sistema
    while (1) {
        // TODO: [Pessoa 3] Implementar lógica do loop principal
        // Exemplo: chamar o dispatcher para gerenciar processos
    }

    return 0; // Retornar 0 ao finalizar
}
