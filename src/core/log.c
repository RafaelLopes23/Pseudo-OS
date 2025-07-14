#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void print_divider(char fill) {
    char line[LOG_WIDTH + 1];
    memset(line, fill, LOG_WIDTH);
    line[LOG_WIDTH] = '\0';
    printf("%s\n", line);
}

// *** FUNÇÃO ATUALIZADA PARA SUPORTAR MÚLTIPLAS LINHAS ***
void print_log_entry(const char *col1, const char *col2) {
    const int col1_width = COL1_WIDTH - 3;
    const int col2_width = COL2_WIDTH - 3;
    const char *p2 = col2;
    int first_line = 1;

    while (strlen(p2) > 0) {
        char col2_buffer[col2_width + 1];
        strncpy(col2_buffer, p2, col2_width);
        col2_buffer[col2_width] = '\0';

        if (first_line) {
            printf("│ %-*s │ %-*s │\n", col1_width, col1, col2_width, col2_buffer);
            first_line = 0;
        } else {
            // Para as linhas seguintes, a primeira coluna fica em branco
            printf("│ %-*s │ %-*s │\n", col1_width, "", col2_width, col2_buffer);
        }

        p2 += strlen(col2_buffer);
    }
}


void print_section_header(const char *title) {
    print_divider('=');
    printf("╞ %-*s ╡\n", LOG_WIDTH - 4, title);
    print_divider('=');
}

void log_operation_result(int pid, const char *operation, const char *filename, int success, const char* details) {
    char col1[COL1_WIDTH];
    // Buffer grande para garantir que a mensagem completa seja montada
    char full_message[512]; 
    const char* symbol;
    const char* status_text;

    switch (success) {
        case 1: 
            symbol = "✅";
            status_text = "SUCCESS";
            break;
        case 0:
            symbol = "❌";
            status_text = "ERROR";
            break;
        default:
            symbol = "⚠️";
            status_text = "IGNORED";
            break;
    }

    snprintf(col1, COL1_WIDTH, "PID %d", pid);

    if (details) {
        snprintf(full_message, sizeof(full_message), "%s [%s] %s: '%s' - %s", 
                 symbol, status_text, operation, filename, details);
    } else {
        snprintf(full_message, sizeof(full_message), "%s [%s] %s: '%s'", 
                 symbol, status_text, operation, filename);
    }
    
    print_log_entry(col1, full_message);
    print_divider('-');
}

void log_simulation_outcome(bool success) {
    char col1[COL1_WIDTH], col2[COL2_WIDTH];
    const char* symbol = success ? "🏆" : "⚡";
    const char* message = success ? "Simulação Concluída com Sucesso" : "Simulação Interrompida";

    print_section_header("RESULTADO FINAL");
    
    snprintf(col1, COL1_WIDTH, "%s", symbol);
    snprintf(col2, COL2_WIDTH, "%s", message);
    
    print_log_entry(col1, col2);
    print_divider('=');
}