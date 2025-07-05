#include "resources.h"
#include "../../include/shared/constants.h"
#include <string.h>

IOResources io_resources;

void init_io_resources(IOResources *io_resources) {
    io_resources->scanner_available     = NUMBER_OF_SCANNERS;
    io_resources->printers_available    = NUMBER_OF_PRINTERS;
    io_resources->modem_available       = NUMBER_OF_MODEMS;
    io_resources->sata_drives_available = NUMBER_OF_SATA_DRIVES;
}

int allocate_io_resource(IOResources *io_resources, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        if (io_resources->scanner_available > 0) { io_resources->scanner_available--; return 0; }
    } else if (strcmp(type, "printer") == 0) {
        if (io_resources->printers_available > 0) { io_resources->printers_available--; return 0; }
    } else if (strcmp(type, "modem") == 0) {
        if (io_resources->modem_available > 0) { io_resources->modem_available--; return 0; }
    } else if (strcmp(type, "sata") == 0) {
        if (io_resources->sata_drives_available > 0) { io_resources->sata_drives_available--; return 0; }
    }
    return -1;
}

void release_io_resource(IOResources *io_resources, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        if (io_resources->scanner_available < NUMBER_OF_SCANNERS) io_resources->scanner_available++;
    } else if (strcmp(type, "printer") == 0) {
        if (io_resources->printers_available < NUMBER_OF_PRINTERS) io_resources->printers_available++;
    } else if (strcmp(type, "modem") == 0) {
        if (io_resources->modem_available < NUMBER_OF_MODEMS) io_resources->modem_available++;
    } else if (strcmp(type, "sata") == 0) {
        if (io_resources->sata_drives_available < NUMBER_OF_SATA_DRIVES) 
            io_resources->sata_drives_available++;
    }
}

int check_io_resource_availability(IOResources *io_resources, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        return io_resources->scanner_available > 0;
    } else if (strcmp(type, "printer") == 0) {
        return io_resources->printers_available > 0;
    } else if (strcmp(type, "modem") == 0) {
        return io_resources->modem_available > 0;
    } else if (strcmp(type, "sata") == 0) {
        return io_resources->sata_drives_available > 0;
    }
    return 0;
}