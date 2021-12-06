#ifndef DISKORGANIZATION_STRUCT_H
#define DISKORGANIZATION_STRUCT_H

#include <stdint.h>

#define entry_value_unused          0x000
#define entry_value_reserved_start  0xff0
#define entry_value_reserved_end    0xff6
#define entry_value_bad_cluster     0xff7
#define entry_value_ending_start    0xff8
#define entry_value_ending_end      0xfff

struct DiskOrganization
{
    unsigned int bytes_per_sector;
    unsigned int sectors_per_cluster;
    unsigned int sectors_per_fat;
    unsigned int sectors_per_track;

    unsigned int number_of_reserved_sectors;
    unsigned int number_of_heads;
    
    unsigned int max_root_directory_entries;
    unsigned int total_sector_count;

    unsigned int volume_id;
    unsigned int volum_label;
    
    char* file_sys_type;
    char * system;
};

struct DiskOrganization * CreateDiskConstructor(uint8_t *header);

#endif