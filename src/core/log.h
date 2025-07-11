#ifndef LOG_H
#define LOG_H

#define LOG_WIDTH     100  // Aumentado para 100 caracteres
#define COL1_WIDTH    22   // Largura para a primeira coluna
#define COL2_WIDTH    74   // Largura para a segunda coluna (LOG_WIDTH - COL1_WIDTH - 4)
#define FILENAME_LEN  30   // Tamanho máximo para nomes de arquivo
#define NUM_COL_WIDTH 10   // Largura para colunas numéricas


// Usando char para compatibilidade com memset
void print_divider(char fill);
void print_log_entry(const char *col1, const char *col2);
void print_section_header(const char *title);
void log_operation_result(int pid, const char *operation, const char *filename, int);

#endif