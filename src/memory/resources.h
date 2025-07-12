#ifndef RESOURCES_H
#define RESOURCES_H

#include "../core/semaphore.h"
#include "../../include/shared/constants.h"

// Estrutura de dados para gerenciamento de recursos de E/S usando semáforos
typedef struct {
    SimulatedSemaphore scanner_sem;
    SimulatedSemaphore printer_sem;
    SimulatedSemaphore modem_sem;
    SimulatedSemaphore sata_drive_sem;
} IOResources;

extern IOResources io_resources;

// Função para inicializar os recursos de E/S
void init_io_resources(IOResources *resources);

// Função para tentar alocar um recurso de E/S. Retorna 0 em sucesso, 1 se bloqueado.
int allocate_io_resource(PCB* process, const char *resource_type);

// Função para liberar um recurso de E/S
void release_io_resource(const char *resource_type);

// Função para verificar a disponibilidade (usada para a verificação inicial)
int check_io_resource_availability(const char *resource_type);

#endif // RESOURCES_H