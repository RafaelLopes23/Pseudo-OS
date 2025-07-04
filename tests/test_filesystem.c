#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../src/filesystem/filesystem.h"
#include "../src/filesystem/fileops.h"

void test_create_file() {
    printf("=== Teste: Criação de Arquivo ===\n");
    
    // Inicializar sistema de arquivos
    init_filesystem();
    
    // Testar criação de arquivo
    int result = create_file("test1.txt", 5);
    assert(result == 0);
    
    // Verificar se arquivo foi criado
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, "test1.txt") == 0) {
            found = 1;
            assert(fs.files[i].block_count == 5);
            break;
        }
    }
    assert(found);
    
    printf("✓ Criação de arquivo funcionando corretamente\n");
}

void test_delete_file() {
    printf("\n=== Teste: Deleção de Arquivo ===\n");
    
    // Criar arquivo primeiro
    create_file("test2.txt", 3);
    
    // Deletar arquivo
    int result = delete_file("test2.txt");
    assert(result == 0);
    
    // Verificar se arquivo foi deletado
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (strcmp(fs.files[i].name, "test2.txt") == 0) {
            found = 1;
            break;
        }
    }
    assert(!found);
    
    printf("✓ Deleção de arquivo funcionando corretamente\n");
}

void test_read_file() {
    printf("\n=== Teste: Leitura de Arquivo ===\n");
    
    // Criar arquivo
    create_file("test3.txt", 2);
    
    // Tentar ler arquivo
    char buffer[256];
    int result = read_file("test3.txt", buffer, sizeof(buffer));
    assert(result == 0);
    
    printf("✓ Leitura de arquivo funcionando corretamente\n");
}

void test_write_file() {
    printf("\n=== Teste: Escrita em Arquivo ===\n");
    
    // Criar arquivo
    create_file("test4.txt", 3);
    
    // Escrever dados no arquivo
    const char* data = "Dados de teste";
    int result = write_file("test4.txt", data, strlen(data));
    assert(result == 0);
    
    printf("✓ Escrita em arquivo funcionando corretamente\n");
}

int main() {
    printf("===== TESTES DO SISTEMA DE ARQUIVOS =====\n");
    
    test_create_file();
    test_delete_file();
    test_read_file();
    test_write_file();
    
    printf("\n===== TODOS OS TESTES CONCLUÍDOS =====\n");
    return 0;
}