#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char * GetOSName(char *ptr);
char * GetDiskLabel(char *ptr);
unsigned int GetNumFile(char *root, char *file, int counter_l);
unsigned long int GetDiskSize(char * ptr);
unsigned long int GetFreeDiskSize(char * ptr);
char * GetFileName(char * file_entry);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Invalid number of arguments\n");
        exit(EXIT_FAILURE);
    }

    int f = open(argv[1], O_RDWR);
    if(f < 0)
    {
        fprintf(stderr, "Failed to read disk image %s.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    struct stat buffer;
    if(fstat(f, &buffer) == -1)
    {
        fprintf(stderr, "File Size Un-retrievable\n");
        exit(EXIT_FAILURE);
    }
    
    char *char_ptr = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, f, 0);
    if (char_ptr == MAP_FAILED)
    {
        fprintf(stderr, "Mapping IMA to memory failed\n");
        exit(EXIT_FAILURE);
    }
    
    char * os_name = GetOSName(char_ptr);
    char * label_name = GetDiskLabel(char_ptr);
    unsigned long int disk_size_total = GetDiskSize(char_ptr);
    unsigned long int disk_size_free = GetFreeDiskSize(char_ptr);
    unsigned int num_file = GetNumFile(char_ptr, char_ptr + 512 * 19, 14);
    unsigned int num_of_fat_copies = (unsigned int) char_ptr[16];
    unsigned int sec_per_fat = (unsigned int) char_ptr[23] << 8 | char_ptr[22];

    fprintf(stdout, "OS Name: %s\n", os_name);
    fprintf(stdout, "Label of the disk: %s\n", label_name);
    fprintf(stdout, "Total size of the disk: %ld\n", disk_size_total);
    fprintf(stdout, "Free size of the disk: %ld\n\n", disk_size_free);
    fprintf(stdout, "==============\n");
    fprintf(stdout, "The number of files in the disk: %d\n\n", num_file);
    fprintf(stdout, "=============\nNumber of FAT copies: %d\n", num_of_fat_copies);
    fprintf(stdout, "Sectors per FAT: %d\n", sec_per_fat);

    munmap(char_ptr, 0);
    exit(EXIT_SUCCESS);
    return 0;
}

char * GetOSName(char *ptr)
{
    char * res;

    if((res = malloc(sizeof(char) * 8)) == NULL)
    {
        fprintf(stderr, "Malloc failed\n");
        exit(EXIT_FAILURE);
    }

    ptr += 3;
    for(int i = 0; i < 8; i++)
    {
        res[i] = ptr[i];
    }

    return res;
}

unsigned long int GetDiskSize(char * ptr)
{
    unsigned int bytes_per_sector;
    unsigned int num_of_sector;
    unsigned long int total_size_disk;
    bytes_per_sector = (unsigned int) (ptr[0x0B + 1] << 8 | ptr[0x0B]);    
    num_of_sector = (unsigned int) (ptr[0x13 + 1] << 8 | ptr[0x13]);
    total_size_disk = bytes_per_sector * num_of_sector;

    return total_size_disk;
}

unsigned long int GetFreeDiskSize(char * ptr)
{
    unsigned int bytes_per_sector;
    unsigned int num_of_sector;
    unsigned int num_of_free_sector;
    unsigned int buf1, buf2, buf3;
    unsigned long int total_size_disk;
    
    bytes_per_sector = (unsigned long int) (ptr[0x0B + 1] << 8 | ptr[0x0B]);
    num_of_sector = (unsigned long int) (ptr[0x13 + 1] << 8 | ptr[0x13]); 
    num_of_sector -= 33 + 2;
    num_of_free_sector = 0;

    for(int i = 0; i < num_of_sector; i++)
    {
        if (i % 2 == 0)
        {
            buf1 = ptr[bytes_per_sector + (3 * i) / 2 + 1] & 0x0f;
            buf2 = ptr[bytes_per_sector + (3 * i) / 2] & 0xff;
            buf3 = (buf1 << 8) | buf2;
        }
        else
        {
            buf1 = ptr[bytes_per_sector + (3 * i) / 2] & 0xF0; 
            buf2 = ptr[bytes_per_sector + (3 * i) / 2 + 1] & 0xFF;
            buf3 = (buf1 >> 4) | (buf2 << 4);
        }

        if (buf3 == 0) num_of_free_sector++;
    }
    
    total_size_disk = bytes_per_sector * num_of_free_sector;

    return total_size_disk;
}

int GetEntryValue(char *root, int entry_val)
{
    int tmp1, tmp0, res;

    if (entry_val % 2 == 0)
    {
        tmp1 = root[512 + 1 + (3 * entry_val / 2)] & 0x0F;
        tmp0 = root[512 + (3 * entry_val / 2)] & 0xFF;
        res = (tmp1 << 8) | (tmp0);
    }
    else
    {
        tmp1 = root[512 + (3 * entry_val / 2)] & 0xF0;
        tmp0 = root[512 + 1 + (3 * entry_val / 2)] & 0xFF;
        res = (tmp1 >> 4) | (tmp0 << 4);
    }

    return res;
}

unsigned int GetNumFile(char *root, char *file, int counter_l)
{
    unsigned int res = 0;

    int counter = 0;
    while(file[0] != 0x00 && counter++ < counter_l)
    {
        unsigned int first_logc = (file[26] & 0x00ff) + ((file[27] & 0x00ff) << 8);
        int subdirectory_flag = (file[11] & 0x10) == 0x10;
        int dot_file_flag = file[0] == '.';
        int volume_flag = (file[11] & 0x08) == 0x08;

        if(!subdirectory_flag && first_logc > 1)
        {
            res++;
        }
        else if(subdirectory_flag && !dot_file_flag && !volume_flag)
        {
            int entry_val = (file[26] & 0xff) + ((file[27] & 0xff) << 8);
            res += GetNumFile(root, root + (33 + first_logc - 2) * 512 + 32, 16);
        }

        file += 32;
    }

    return res;
}

char * GetFileName(char * file_entry)
{
    char * dest = malloc(sizeof(char) * 13);
    if(dest == NULL)
    {
        fprintf(stderr, "failed to malloc memory");
        exit(EXIT_FAILURE);
    }

    int i;
    for(i = 0; i < 8; i++)
    {
        if(file_entry[i] == ' ')
        {
            break;
        }

        dest[i] = file_entry[i];
    }

    dest[i++] = '.';

    for(int j = 0; j < 3; j++)
    {
        if(file_entry[j + 8] == ' ')
        {
            break;
        }

        dest[i++] = file_entry[j + 8];
    }
    dest[i] = '\0';
    return dest;
}

char * GetDiskLabel(char *ptr)
{
    unsigned int bytes_per_sec = (unsigned int)(ptr[0x0B + 1] << 8 | ptr[0x0B]);
    ptr += bytes_per_sec * 19;

    while(ptr[0] != 0)
    {
        if ((ptr[11] & 0x08) == 0x08)
        {
            return GetFileName(ptr);
        }

        ptr += 32;
    }

    return "Void";
}