// resources.h
#ifndef RESOURCES_H
#define RESOURCES_H

// TODO: [Pessoa 2] Implementar gerenciamento de recursos E/S

// Estruturas de dados para gerenciamento de recursos
typedef struct {
    int scanner_available; // 1 se disponível, 0 se em uso
    int printers_available; // Contador de impressoras disponíveis
    int modem_available; // 1 se disponível, 0 se em uso
    int sata_drives_available; // Contador de drives SATA disponíveis
} IOResources;

extern IOResources io_resources;

// Função para inicializar os recursos de E/S
void init_io_resources(IOResources *resources);

// Função para alocar um recurso de E/S
int allocate_io_resource(IOResources *resources, const char *resource_type);

// Função para liberar um recurso de E/S
void release_io_resource(IOResources *resources, const char *resource_type);

// Função para verificar a disponibilidade de um recurso de E/S
int check_io_resource_availability(IOResources *resources, const char *resource_type);

#endif // RESOURCES_H