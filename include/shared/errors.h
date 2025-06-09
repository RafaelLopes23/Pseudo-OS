// errors.h
#ifndef ERRORS_H
#define ERRORS_H

// TODO: Definir códigos de erro para o sistema
#define ERR_SUCCESS 0
#define ERR_FAILURE -1
#define ERR_INVALID_PARAM -2
#define ERR_OUT_OF_MEMORY -3
#define ERR_FILE_NOT_FOUND -4
#define ERR_ACCESS_DENIED -5

// TODO: Implementar funções de manipulação de erros
/**
 * @brief Função para obter a descrição de um código de erro.
 * 
 * @param error_code Código de erro.
 * @return Descrição do erro como string.
 */
const char* get_error_description(int error_code);

// TODO: [Pessoa 3] Implementar função para registrar erros em log
/**
 * @brief Função para registrar um erro em log.
 * 
 * @param error_code Código de erro a ser registrado.
 */
void log_error(int error_code);

#endif // ERRORS_H