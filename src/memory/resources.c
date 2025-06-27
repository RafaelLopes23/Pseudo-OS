// src/memory/resources.c
#include "resources.h"
#include "../../include/shared/constants.h"
#include <string.h> // Adicionado para strcmp

// TODO: [Pessoa 2] Implementar gerenciador de recursos E/S
IOResources io_resources;

// Função para inicializar os recursos e OS SEMÁFOROS
void init_io_resources(IOResources *resources) {
    resources->scanner_available = 1;
    resources->printers_available = NUMBER_OF_PRINTERS;
    resources->modem_available = 1;
    resources->sata_drives_available = NUMBER_OF_SATA_DRIVES;

    // Inicializa os semáforos
    // sem_init(sem, pshared, value)
    // pshared = 0 -> semáforo compartilhado entre threads do mesmo processo
    sem_init(&resources->sem_scanner, 0, NUMBER_OF_SCANNERS); // value = 1
    sem_init(&resources->sem_printers, 0, NUMBER_OF_PRINTERS); // value = 2
    sem_init(&resources->sem_modem, 0, NUMBER_OF_MODEMS);       // value = 1
    sem_init(&resources->sem_sata_drives, 0, NUMBER_OF_SATA_DRIVES); // value = 2
}

// Função para destruir os semáforos no fim da execução
void destroy_io_resources(IOResources *resources) {
    sem_destroy(&resources->sem_scanner);
    sem_destroy(&resources->sem_printers);
    sem_destroy(&resources->sem_modem);
    sem_destroy(&resources->sem_sata_drives);
}


// Função para alocar recursos USANDO SEMÁFOROS
// Esta função agora é "thread-safe"
int allocate_io_resource(IOResources *resources, const char *resource_type) {
    // Para manter a lógica original de não bloquear indefinidamente,
    // usaremos sem_trywait, que retorna -1 se não conseguir pegar o recurso.
    // sem_wait bloquearia a thread até o recurso ficar disponível.
    
    if (strcmp(resource_type, "scanner") == 0) {
        if (sem_trywait(&resources->sem_scanner) == 0) { // Tenta pegar o lock
            resources->scanner_available--; // Atualiza contador
            return 0; // Sucesso
        }
    }
    if (strcmp(resource_type, "printer") == 0) {
        if (sem_trywait(&resources->sem_printers) == 0) {
            resources->printers_available--;
            return 0;
        }
    }
    if (strcmp(resource_type, "modem") == 0) {
        if (sem_trywait(&resources->sem_modem) == 0) {
            resources->modem_available--;
            return 0;
        }
    }
    if (strcmp(resource_type, "sata") == 0) {
        if (sem_trywait(&resources->sem_sata_drives) == 0) {
            resources->sata_drives_available--;
            return 0;
        }
    }

    return -1; // Recurso indisponível
}

// Função para liberar recursos USANDO SEMÁFOROS
// Esta função também é "thread-safe"
void release_io_resource(IOResources *resources, const char *resource_type) {
    if (strcmp(resource_type, "scanner") == 0) {
        resources->scanner_available++;
        sem_post(&resources->sem_scanner); // Libera o recurso
    }
    if (strcmp(resource_type, "printer") == 0) {
        resources->printers_available++;
        sem_post(&resources->sem_printers); // Libera o recurso
    }
    if (strcmp(resource_type, "modem") == 0) {
        resources->modem_available++;
        sem_post(&resources->sem_modem); // Libera o recurso
    }
    if (strcmp(resource_type, "sata") == 0) {
        resources->sata_drives_available++;
        sem_post(&resources->sem_sata_drives); // Libera o recurso
    }
}


// A função de verificação não precisa de semáforo, pois é apenas uma leitura
// e não modifica estado. O estado real é garantido pela alocação atômica.
int check_io_resource_availability(IOResources *resources, const char *resource_type) {
    if (strcmp(resource_type, "scanner") == 0) return resources->scanner_available > 0;
    if (strcmp(resource_type, "printer") == 0) return resources->printers_available > 0;
    if (strcmp(resource_type, "modem") == 0) return resources->modem_available > 0;
    if (strcmp(resource_type, "sata") == 0) return resources->sata_drives_available > 0;
    return 0;
}