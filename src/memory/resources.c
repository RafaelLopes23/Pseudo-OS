#include "resources.h"
#include "../../include/shared/constants.h"
#include <string.h>

IOResources io_resources;  // para uso dos testes

void init_io_resources(IOResources *res) {
    res->scanner_available     = NUMBER_OF_SCANNERS;
    res->printers_available    = NUMBER_OF_PRINTERS;
    res->modem_available       = NUMBER_OF_MODEMS;
    res->sata_drives_available = NUMBER_OF_SATA_DRIVES;
}

int allocate_io_resource(IOResources *res, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        if (res->scanner_available > 0) { res->scanner_available--; return 0; }
    } else if (strcmp(type, "printer") == 0) {
        if (res->printers_available > 0) { res->printers_available--; return 0; }
    } else if (strcmp(type, "modem") == 0) {
        if (res->modem_available > 0) { res->modem_available--; return 0; }
    } else if (strcmp(type, "sata") == 0) {
        if (res->sata_drives_available > 0) { res->sata_drives_available--; return 0; }
    }
    return -1;
}

void release_io_resource(IOResources *res, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        if (res->scanner_available < NUMBER_OF_SCANNERS) res->scanner_available++;
    } else if (strcmp(type, "printer") == 0) {
        if (res->printers_available < NUMBER_OF_PRINTERS) res->printers_available++;
    } else if (strcmp(type, "modem") == 0) {
        if (res->modem_available < NUMBER_OF_MODEMS) res->modem_available++;
    } else if (strcmp(type, "sata") == 0) {
        if (res->sata_drives_available < NUMBER_OF_SATA_DRIVES) 
            res->sata_drives_available++;
    }
}

int check_io_resource_availability(IOResources *res, const char *type) {
    if (strcmp(type, "scanner") == 0) {
        return res->scanner_available > 0;
    } else if (strcmp(type, "printer") == 0) {
        return res->printers_available > 0;
    } else if (strcmp(type, "modem") == 0) {
        return res->modem_available > 0;
    } else if (strcmp(type, "sata") == 0) {
        return res->sata_drives_available > 0;
    }
    return 0;
}