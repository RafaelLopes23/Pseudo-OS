// system.h
#ifndef SYSTEM_H
#define SYSTEM_H

// TODO: [Pessoa 3] Implementar funções de inicialização do sistema e gerenciamento geral

// Função para inicializar o sistema
void initialize_system();

// Função para iniciar o loop principal do sistema
void start_main_loop();

// Função para encerrar o sistema
void shutdown_system();

// Definições de constantes do sistema
#define MAX_PROCESSES 1000
#define MAX_MEMORY_BLOCKS 1024

#endif // SYSTEM_H