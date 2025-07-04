#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "types.h"

// adiciona includes para memória, recursos e filesystem
#include "../filesystem/filesystem.h"
#include "../memory/memory.h"
#include "../memory/resources.h"
#include "../process/process.h"
#include "../process/dispatcher.h"

static int system_initialized = 0;

void initialize_system() {
    if (system_initialized) return;

    init_filesystem();                 // Pessoa 3
    initialize_memory(&mem_manager);   // Pessoa 2
    init_io_resources(&io_resources);  // Pessoa 2
    init_process();                    // Pessoa 1
    init_scheduler();                  // Pessoa 1

    system_initialized = 1;
    printf("Sistema inicializado\n");
}

void start_main_loop() {
    while (1) {
        PCB* p = dispatch_process();
        if (p) run_current_process();
        usleep(100000);
    }
}

void shutdown_system() {
    printf("Encerrando sistema...\n");
    system_initialized = 0;
}

int main() {
    initialize_system();
    start_main_loop();
    return 0;
}