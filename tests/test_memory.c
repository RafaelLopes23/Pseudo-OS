// tests/test_memory.c
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../src/memory/memory.h"
#include "../src/memory/resources.h"

extern IOResources io_resources;

// Teste: Inicialização da memória
void test_initialize_memory() {
    initialize_memory(&mem_manager);
    for (int i = 0; i < TOTAL_MEMORY_BLOCKS; i++) {
        assert(mem_manager.blocks[i] == 0);
    }
    printf("Teste: Inicialização da memória - OK\n");
}

// Teste: Alocação de memória para usuário
void test_allocate_user_memory() {
    initialize_memory(&mem_manager);
    int offset = allocate_memory(10, 0);  // 10 blocos em área de usuário
    assert(offset >= REAL_TIME_MEMORY_BLOCKS);
    for (int i = offset; i < offset + 10; i++) {
        assert(mem_manager.blocks[i] == 1);
    }
    printf("Teste: Alocação de memória de usuário - OK\n");
}

// Teste: Alocação de memória para tempo real
void test_allocate_real_time_memory() {
    initialize_memory(&mem_manager);
    int offset = allocate_memory(5, 1);  // 5 blocos em área de tempo real
    assert(offset >= 0 && offset < REAL_TIME_MEMORY_BLOCKS);
    for (int i = offset; i < offset + 5; i++) {
        assert(mem_manager.blocks[i] == 1);
    }
    printf("Teste: Alocação de memória tempo real - OK\n");
}

// Teste: Liberação de memória
void test_free_memory() {
    initialize_memory(&mem_manager);
    int offset = allocate_memory(8, 0);
    assert(offset >= REAL_TIME_MEMORY_BLOCKS);

    free_memory(offset, 8);
    for (int i = offset; i < offset + 8; i++) {
        assert(mem_manager.blocks[i] == 0);
    }
    printf("Teste: Liberação de memória - OK\n");
}

// Teste: Verificar disponibilidade de memória
void test_check_memory_availability() {
    initialize_memory(&mem_manager);
    int available = check_memory_availability(10, 0);
    assert(available == 1);

    allocate_memory(USER_MEMORY_BLOCKS, 0);  // Preenche toda a memória de usuário
    available = check_memory_availability(1, 0);
    assert(available == 0);
    printf("Teste: Verificação de disponibilidade de memória - OK\n");
}

// Teste: Inicialização dos recursos de I/O
void test_initialize_io_resources() {
    init_io_resources(&io_resources);
    assert(io_resources.scanner_available == 1);
    assert(io_resources.printers_available == NUMBER_OF_PRINTERS);
    assert(io_resources.modem_available == 1);
    assert(io_resources.sata_drives_available == NUMBER_OF_SATA_DRIVES);
    printf("Teste: Inicialização dos recursos de I/O - OK\n");
}

// Teste: Alocação e liberação de recursos de I/O
void test_io_resource_allocation_and_release() {
    init_io_resources(&io_resources);

    assert(allocate_io_resource(&io_resources, "scanner") == 0);
    assert(io_resources.scanner_available == 0);

    assert(allocate_io_resource(&io_resources, "printer") == 0);
    assert(io_resources.printers_available == NUMBER_OF_PRINTERS - 1);

    assert(allocate_io_resource(&io_resources, "modem") == 0);
    assert(io_resources.modem_available == 0);

    assert(allocate_io_resource(&io_resources, "sata") == 0);
    assert(io_resources.sata_drives_available == NUMBER_OF_SATA_DRIVES - 1);

    release_io_resource(&io_resources, "scanner");
    assert(io_resources.scanner_available == 1);

    release_io_resource(&io_resources, "printer");
    assert(io_resources.printers_available == NUMBER_OF_PRINTERS);

    release_io_resource(&io_resources, "modem");
    assert(io_resources.modem_available == 1);

    release_io_resource(&io_resources, "sata");
    assert(io_resources.sata_drives_available == NUMBER_OF_SATA_DRIVES);

    printf("Teste: Alocação e liberação de recursos de I/O - OK\n");
}

// Teste: Verificação de disponibilidade de recursos de I/O
void test_check_io_resource_availability() {
    init_io_resources(&io_resources);
    assert(check_io_resource_availability(&io_resources, "scanner") == 1);
    assert(check_io_resource_availability(&io_resources, "printer") == 1);
    assert(check_io_resource_availability(&io_resources, "modem") == 1);
    assert(check_io_resource_availability(&io_resources, "sata") == 1);

    allocate_io_resource(&io_resources, "scanner");
    assert(check_io_resource_availability(&io_resources, "scanner") == 0);

    printf("Teste: Verificação de disponibilidade de recursos de I/O - OK\n");
}

int main() {
    printf("Iniciando testes de memória e recursos...\n\n");

    test_initialize_memory();
    test_allocate_user_memory();
    test_allocate_real_time_memory();
    test_free_memory();
    test_check_memory_availability();

    test_initialize_io_resources();
    test_io_resource_allocation_and_release();
    test_check_io_resource_availability();

    printf("\nTodos os testes passaram com sucesso!\n");
    return 0;
}
