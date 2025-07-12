#include "resources.h"
#include <string.h>

IOResources io_resources;

void init_io_resources(IOResources *resources) {
    init_semaphore(&resources->scanner_sem, NUMBER_OF_SCANNERS);
    init_semaphore(&resources->printer_sem, NUMBER_OF_PRINTERS);
    init_semaphore(&resources->modem_sem, NUMBER_OF_MODEMS);
    init_semaphore(&resources->sata_drive_sem, NUMBER_OF_SATA_DRIVES);
}

// Tenta alocar um recurso para um processo
int allocate_io_resource(PCB* process, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        return sem_wait(&io_resources.scanner_sem, process);
    } else if (strcmp(type, "printer") == 0) {
        return sem_wait(&io_resources.printer_sem, process);
    } else if (strcmp(type, "modem") == 0) {
        return sem_wait(&io_resources.modem_sem, process);
    } else if (strcmp(type, "sata") == 0) {
        return sem_wait(&io_resources.sata_drive_sem, process);
    }
    return -1; // Tipo de recurso inválido
}

// Libera um recurso
void release_io_resource(const char *type) {
    if (strcmp(type, "scanner") == 0) {
        sem_signal(&io_resources.scanner_sem);
    } else if (strcmp(type, "printer") == 0) {
        sem_signal(&io_resources.printer_sem);
    } else if (strcmp(type, "modem") == 0) {
        sem_signal(&io_resources.modem_sem);
    } else if (strcmp(type, "sata") == 0) {
        sem_signal(&io_resources.sata_drive_sem);
    }
}

// Verifica a disponibilidade inicial sem bloquear
int check_io_resource_availability(const char *type) {
    if (strcmp(type, "scanner") == 0) {
        return io_resources.scanner_sem.value > 0;
    } else if (strcmp(type, "printer") == 0) {
        return io_resources.printer_sem.value > 0;
    } else if (strcmp(type, "modem") == 0) {
        return io_resources.modem_sem.value > 0;
    } else if (strcmp(type, "sata") == 0) {
        return io_resources.sata_drive_sem.value > 0;
    }
    return 0;
}