#ifndef DISK_STRUCT_H
#define DISK_STRUCT_H

#include <stdio.h>

#include "BootSector.h"
#include "DiskOrganization.h"
#include "FileOrganization.h"

struct Disk
{
    struct DiskOrganization     *disk;
    struct FileOrganization     *file;
    struct DiskBootSector       *boot;
};

struct Disk * DiskConstructor(FILE *f);

#endif