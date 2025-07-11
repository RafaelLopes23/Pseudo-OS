#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "process.h"

// Funções do dispatcher
PCB* dispatch_process();
void run_current_process();
PCB* get_current_process();

#endif // DISPATCHER_H