#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

// TODO: [Pessoa 3] Implementar testes para o sistema de arquivos

void test_create_file() {
    // TODO: Implementar teste para criação de arquivo
    // Exemplo: Chamar a função de criação de arquivo e verificar se o arquivo foi criado corretamente
}

void test_delete_file() {
    // TODO: Implementar teste para deleção de arquivo
    // Exemplo: Chamar a função de deleção de arquivo e verificar se o arquivo foi removido corretamente
}

void test_read_file() {
    // TODO: Implementar teste para leitura de arquivo
    // Exemplo: Chamar a função de leitura de arquivo e verificar se os dados estão corretos
}

void test_write_file() {
    // TODO: Implementar teste para escrita em arquivo
    // Exemplo: Chamar a função de escrita de arquivo e verificar se os dados foram gravados corretamente
}

int main() {
    // Executar todos os testes
    test_create_file();
    test_delete_file();
    test_read_file();
    test_write_file();

    printf("Todos os testes do sistema de arquivos foram executados.\n");
    return 0;
}