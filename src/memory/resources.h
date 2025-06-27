// resources.h
#ifndef RESOURCES_H
#define RESOURCES_H

#include <semaphore.h> // <--- INCLUIR BIBLIOTECA

// Estruturas de dados para gerenciamento de recursos
typedef struct {
    // Os contadores agora são controlados pelos semáforos,
    // mas podem ser mantidos para fins de depuração ou lógica adicional se necessário.
    int scanner_available;
    int printers_available;
    int modem_available;
    int sata_drives_available;

    // Semáforos para garantir acesso exclusivo e contagem
    sem_t sem_scanner;
    sem_t sem_printers;
    sem_t sem_modem;
    sem_t sem_sata_drives;

} IOResources;

// Função para inicializar os recursos de E/S
void init_io_resources(IOResources *resources);

// Função para destruir os semáforos ao final do programa
void destroy_io_resources(IOResources *resources);

// Função para alocar um recurso de E/S
int allocate_io_resource(IOResources *resources, const char *resource_type);

// Função para liberar um recurso de E/S
void release_io_resource(IOResources *resources, const char *resource_type);

// Função para verificar a disponibilidade de um recurso de E/S
int check_io_resource_availability(IOResources *resources, const char *resource_type);

#endif // RESOURCES_H