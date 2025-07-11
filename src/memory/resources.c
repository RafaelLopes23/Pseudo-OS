#include "resources.h"
#include "../../include/shared/constants.h"
#include <string.h>
#include <pthread.h>

IOResources io_resources;

void init_io_resources(IOResources *io_resources) {
    io_resources->scanner_available = NUMBER_OF_SCANNERS;
    io_resources->printers_available = NUMBER_OF_PRINTERS;
    io_resources->modem_available = NUMBER_OF_MODEMS;
    io_resources->sata_drives_available = NUMBER_OF_SATA_DRIVES;
    pthread_mutex_init(&io_resources->lock, NULL);
}

void destroy_io_resources() {
    pthread_mutex_destroy(&io_resources.lock);
}

int allocate_io_resource(IOResources *io_resources, const char *type) {
    pthread_mutex_lock(&io_resources->lock);
    
    int result = -1;
    if (strcmp(type, "scanner") == 0) {
        if (io_resources->scanner_available > 0) { 
            io_resources->scanner_available--; 
            result = 0;
        }
    } else if (strcmp(type, "printer") == 0) {
        if (io_resources->printers_available > 0) { 
            io_resources->printers_available--; 
            result = 0; 
        }
    } else if (strcmp(type, "modem") == 0) {
        if (io_resources->modem_available > 0) { 
            io_resources->modem_available--; 
            result = 0; 
        }
    } else if (strcmp(type, "sata") == 0) {
        if (io_resources->sata_drives_available > 0) { 
            io_resources->sata_drives_available--; 
            result = 0; 
        }
    }
    
    pthread_mutex_unlock(&io_resources->lock);
    return result;
}

void release_io_resource(IOResources *io_resources, const char *type) {

    pthread_mutex_lock(&io_resources->lock);
    
    if (strcmp(type, "scanner") == 0) {
        if (io_resources->scanner_available < NUMBER_OF_SCANNERS)
            io_resources->scanner_available++;
    } else if (strcmp(type, "printer") == 0) {
        if (io_resources->printers_available < NUMBER_OF_PRINTERS) 
            io_resources->printers_available++;
    } else if (strcmp(type, "modem") == 0) {
        if (io_resources->modem_available < NUMBER_OF_MODEMS) 
            io_resources->modem_available++;
    } else if (strcmp(type, "sata") == 0) {
        if (io_resources->sata_drives_available < NUMBER_OF_SATA_DRIVES) 
            io_resources->sata_drives_available++;
    }

    pthread_mutex_unlock(&io_resources->lock);
}

int check_io_resource_availability(IOResources *io_resources, const char *type) {

    pthread_mutex_lock(&io_resources->lock);
    
    int available = 0;
    if (strcmp(type, "scanner") == 0) {
        available = io_resources->scanner_available > 0;
    } else if (strcmp(type, "printer") == 0) {
        available = io_resources->printers_available > 0;
    } else if (strcmp(type, "modem") == 0) {
        available = io_resources->modem_available > 0;
    } else if (strcmp(type, "sata") == 0) {
        available = io_resources->sata_drives_available > 0;
    }

    pthread_mutex_unlock(&io_resources->lock);
    return available;
}