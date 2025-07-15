#include "export.h"
#include "cJSON.h"
#include "../process/process.h"
#include "../process/scheduler.h"
#include "../process/dispatcher.h" 
#include "../memory/memory.h"
#include "../memory/resources.h"
#include <stdio.h>
#include <stdlib.h> 


// Função auxiliar para adicionar um processo a um array JSON
static void add_process_to_json_array(cJSON *array, PCB* process) {
    if (!process) return;
    cJSON *proc_obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(proc_obj, "pid", process->pid);
    cJSON_AddNumberToObject(proc_obj, "priority", process->priority);
    cJSON_AddNumberToObject(proc_obj, "state", process->state);
    cJSON_AddNumberToObject(proc_obj, "cpu_time", process->cpu_time);
    cJSON_AddNumberToObject(proc_obj, "total_cpu_time", process->total_cpu_time);
    cJSON_AddItemToArray(array, proc_obj);
}

// Função principal de exportação
void export_system_state_to_json(int tick) {
    // Cria o objeto JSON principal
    cJSON *root = cJSON_CreateObject();
    
    // Adiciona informações gerais
    cJSON_AddNumberToObject(root, "tick", tick);
    PCB* current_process = get_current_process();
    if (current_process) {
        cJSON_AddNumberToObject(root, "cpu_current_pid", current_process->pid);
    } else {
        cJSON_AddNullToObject(root, "cpu_current_pid");
    }

    // Adiciona o mapa de memória
    cJSON *mem_array = cJSON_CreateIntArray(mem_manager.blocks, TOTAL_MEMORY_BLOCKS);
    cJSON_AddItemToObject(root, "memory_map", mem_array);

    // Adiciona as filas de processos
    cJSON *queues = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "queues", queues);

    // Fila de Prontos (junta todas as filas de prioridade)
    cJSON *ready_queue_json = cJSON_CreateArray();
    cJSON_AddItemToObject(queues, "ready", ready_queue_json);
    for (int i = 0; i < NUMBER_OF_PRIORITY_QUEUES; i++) {
        ProcessQueue* q = &priority_queues[i];
        int current = q->front;
        for (int j = 0; j < q->count; j++) {
            add_process_to_json_array(ready_queue_json, q->processes[current]);
            current = (current + 1) % MAX_PROCESSES_PER_QUEUE;
        }
    }

    // Fila de Background
    cJSON *background_queue_json = cJSON_CreateArray();
    cJSON_AddItemToObject(queues, "background", background_queue_json);
    int current_bg = background_queue.front;
    for (int j = 0; j < background_queue.count; j++) {
        add_process_to_json_array(background_queue_json, background_queue.processes[current_bg]);
        current_bg = (current_bg + 1) % MAX_PROCESSES_PER_QUEUE;
    }

    // Fila de Suspensos
    cJSON *suspended_queue_json = cJSON_CreateArray();
    cJSON_AddItemToObject(queues, "suspended", suspended_queue_json);
     int current_susp = suspended_queue.front;
    for (int j = 0; j < suspended_queue.count; j++) {
        add_process_to_json_array(suspended_queue_json, suspended_queue.processes[current_susp]);
        current_susp = (current_susp + 1) % MAX_PROCESSES_PER_QUEUE;
    }

    // Transforma o objeto JSON numa string formatada
    char *json_string = cJSON_Print(root);

    // Escreve a string no ficheiro estado.json
    FILE *fp = fopen("estado.json", "w");
    if (fp) {
        fprintf(fp, "%s", json_string);
        fclose(fp);
    }

    // Liberta a memória usada pelo cJSON
    free(json_string);
    cJSON_Delete(root);
}