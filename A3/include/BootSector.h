#ifndef DISKBOOTSECTOR_STRUCT_H
#define DISKBOOTSECTOR_STRUCT_H

#include <stdint.h>

struct DiskBootSector
{
    char * ops_name;
    unsigned int bios_parameter;
};

struct DiskBootSector * DiskBootSectorConstructor(uint8_t *header);

#endif