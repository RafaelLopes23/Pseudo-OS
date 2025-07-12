#include "log.h"
#include <stdio.h>
#include <string.h>

void print_divider(char fill) {
    char line[LOG_WIDTH + 1];
    memset(line, fill, LOG_WIDTH);
    line[LOG_WIDTH] = '\0';
    printf("%s\n", line);
}

void print_log_entry(const char *col1, const char *col2) {
    char safe_col1[COL1_WIDTH + 1];
    char safe_col2[COL2_WIDTH + 1];
    
    strncpy(safe_col1, col1, COL1_WIDTH);
    safe_col1[COL1_WIDTH] = '\0';
    
    strncpy(safe_col2, col2, COL2_WIDTH);
    safe_col2[COL2_WIDTH] = '\0';
    
    printf("│ %-*s │ %-*s │\n", 
           COL1_WIDTH - 3, safe_col1, 
           COL2_WIDTH - 3, safe_col2);
}

void print_section_header(const char *title) {
    print_divider('=');
    printf("╞ %-*s ╡\n", LOG_WIDTH - 4, title);
    print_divider('=');
}

void log_operation_result(int pid, const char *operation, const char *filename, int success, const char* details) {
    char col1[COL1_WIDTH], col2[COL2_WIDTH];
    const char* symbol;
    const char* status_text;

    switch (success) {
        case 1: // Sucesso
            symbol = "✅";
            status_text = "SUCCESS";
            break;
        case 0: // Erro
            symbol = "❌";
            status_text = "ERROR";
            break;
        default: // Ignorado)
            symbol = "⚠️";
            status_text = "IGNORED";
            break;
    }

    snprintf(col1, COL1_WIDTH, "PID %d", pid);

    // impressão para o símbolo
    if (details) {
        snprintf(col2, COL2_WIDTH, "%s [%s] %s: '%s' - %s", 
                 symbol, status_text, operation, filename, details);
    } else {
        snprintf(col2, COL2_WIDTH, "%s [%s] %s: '%s'", 
                 symbol, status_text, operation, filename);
    }
    
    print_log_entry(col1, col2);
    print_divider('-');
}