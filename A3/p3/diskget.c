#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

char * GetFileName(char * file_entry);
int WriteFromMemToFile(
    char *root, FILE *f, unsigned int entry_val, int file_size
);

int main(int argc, char *argv[])
{
    if (argc != 3)
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
    
    char *disk_init = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, f, 0);
    if (disk_init == MAP_FAILED)
    {
        fprintf(stderr, "Mapping IMA to memory failed\n");
        exit(EXIT_FAILURE);
    }
    
    char *tmp = argv[2];
    while(*tmp)
    {
        *tmp = toupper((unsigned int) *tmp);
        tmp++;
    }

    tmp = disk_init + 512 * 19;
    char *file_name, *file_dir_entry;
    int counter = 1;
    int flag = 1;
    while(tmp[0] != 0x00 && counter++ != 14)
    {
        file_name = GetFileName(tmp);
        if(!strcmp(file_name, argv[2]))
        {
            flag--;
            break;
        }
        
        tmp += 32;
    }

    if(flag)
    {
        fprintf(stderr, "%s does not exist at root directory of %s\n", argv[2], argv[1]);
        exit(1);
    }
    
    file_dir_entry = tmp;
    unsigned int file_size = (
        (file_dir_entry[28] & 0x00ff) | ((file_dir_entry[29] & 0x00ff) << 8) |
        ((file_dir_entry[30] & 0x00ff) << 16) | ((file_dir_entry[31] & 0x00ff) << 24)
    );
    unsigned int file_entry = (
        (file_dir_entry[26] & 0x00ff) | (file_dir_entry[27] & 0x00ff) << 8
    );

    FILE *dest_file = fopen(argv[2], "w");
    if(dest_file == NULL)
    {
        fprintf(stderr, "Failed to create/open file: %s\n", argv[2]);
        close(f);
        exit(EXIT_FAILURE);
    }

    if(WriteFromMemToFile(disk_init, dest_file, file_entry, file_size))
    {
        fprintf(stderr, "Failed to write values to the destination file.\n");
        close(f);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
    return 0;
}

unsigned int GetEntryValue(char *root, int entry_val)
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

int WriteFromMemToFile(
    char *root, FILE *f, unsigned int entry_val, int file_size
)
{
    do
    {   
        char *src = &root[(33 + entry_val - 2) * 512];
        
        if(file_size > 512)
        {
            fwrite(src, sizeof(char), 512, f);
        }
        else
        {
            fwrite(src, sizeof(char), file_size, f);
        }

        printf("%.3x\t%.3x\n", 0xfff, entry_val & 0xfff);
        entry_val = GetEntryValue(root, entry_val) & 0xfff;
        file_size -= 512;

    } while (entry_val < 0xff8 && file_size > 0);
    
    return 0;
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