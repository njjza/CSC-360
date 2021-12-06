#ifndef FILEORGANIZATION_STRUCT_H
#define FILEORGANIZATION_STRUCT_H

#include <stdint.h>

#define file_read_only_mask         0x01
#define file_hidden_mask            0x02
#define file_system_mask            0x04
#define file_volume_label_mask      0x10
#define file_archive_mask           0x20

struct FileOrganization
{
    char *file_name;
    char *extension;

    unsigned int creation_time;
    unsigned int creation_date;
    unsigned int last_write_time;
    unsigned int last_write_date;
    unsigned int last_access_date;
    unsigned int first_logical_cluster;

    unsigned int file_size;
};

struct FileOrganization * CreateFileConstructor(uint8_t *header);

#endif