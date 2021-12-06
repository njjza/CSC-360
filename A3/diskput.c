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
#include <time.h>

unsigned long int GetFreeMemorySize(char *ptr);
int DirectoryToStrList(char *directory, char *** directory_parsed);
char * SetDirectoryEntry(char* root, char *dir, struct stat *file, char *name, int itr_max);
void WriteFileToDisk(char *disk_dest, char *disk_dir_entry, char *file_src, int size);
char * DirectoryExplorer(char *root, char ** dir, int len);

void StrToUpper(char *name)
{
    while(*name)
    {
        *name = toupper(*name);
        name++;
    }
}

int main(int argc, char* argv[])
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

    char *char_ptr = mmap(NULL, buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, f, 0);
    if (char_ptr == MAP_FAILED)
    {
        fprintf(stderr, "Mapping IMA to memory failed\n");
        close(f);
        exit(EXIT_FAILURE);
    }

    // parse the directory string
    char **directory_list;
    int len = DirectoryToStrList(argv[2], &directory_list);
    
    // determine the size of the disk
    int f_linux = open(directory_list[len - 1], O_RDONLY);
    if(f_linux < 0)
    {
        fprintf(stderr, "Failed to open file %s.\n", directory_list[len - 1]);
        exit(EXIT_FAILURE);
    }

    if(fstat(f_linux, &buffer) == -1)
    {
        fprintf(stderr, "File Size Un-retrievable\n");
        exit(EXIT_FAILURE);
    }

    unsigned long int free_mem = GetFreeMemorySize(char_ptr);
    if(free_mem - buffer.st_size < 0)
    {
        fprintf(stdout, "Not enough space to write the file\n");
        exit(EXIT_SUCCESS);
    }
    
    char *input = mmap(NULL, buffer.st_size, PROT_READ, MAP_SHARED, f_linux, 0);
    StrToUpper(directory_list[len - 1]);
    if(len == 1)
    {
        char * file_dir = SetDirectoryEntry(
            char_ptr, char_ptr + 512 * 19, &buffer, directory_list[0], 16
        );
        WriteFileToDisk(char_ptr, file_dir, input, buffer.st_size);
    }
    else
    {
        char * file_dir = DirectoryExplorer(char_ptr, directory_list, len - 1);
        file_dir = SetDirectoryEntry(
            char_ptr, file_dir, &buffer, directory_list[len - 1], 16
        );
        WriteFileToDisk(char_ptr, file_dir, input, buffer.st_size);
    }

    close(f);
    close(f_linux);
}

int DirectoryToStrList(char *directory, char *** directory_parsed)
{
    char ** res = malloc(sizeof(char *) * 10);
    int res_num = 0;

    if(res == NULL)
    {
        fprintf(stderr, "Failed to malloc");
        exit(1);
    }

    if (directory[0] != '/')
    {
        res[res_num++] = directory;
        goto direct_to_list_end;
    }

    int lim = 10;
    char *tok = strtok(directory, "/");
    while(tok != NULL)
    {
        res[res_num++] = tok;

        if(res_num >= lim)
        {
            lim *= 2;
            res = realloc(res, (sizeof(char*)) * lim);
            if(res == NULL)
            {
                fprintf(stderr, "reallocation failed\n");
                exit(1);
            }
        }

        tok = strtok(NULL, "/");
    }

    direct_to_list_end:
    *directory_parsed = res;
    return res_num;
}

unsigned int GetEntryVal(char *ptr, unsigned int entry_val)
{
    unsigned int bytes_per_sector = (ptr[0x0B + 1] << 8 | ptr[0x0B]);
    unsigned int tmp1, tmp0, tmp;
    
    if (entry_val % 2 == 0)
    {
        tmp1 = ptr[bytes_per_sector + (3 * entry_val) / 2 + 1] & 0x0f;
        tmp0 = ptr[bytes_per_sector + (3 * entry_val) / 2] & 0xff;
        tmp = (tmp1 << 8) | tmp0;
    }
    else
    {
        tmp1 = ptr[bytes_per_sector + (3 * entry_val) / 2] & 0xF0; 
        tmp0 = ptr[bytes_per_sector + (3 * entry_val) / 2 + 1] & 0xFF;
        tmp = (tmp1 >> 4) | (tmp0 << 4);
    }

    return tmp;
}
unsigned int GetFreeEntryVal(char *ptr)
{
    unsigned int num_of_sector = (unsigned int) (ptr[0x13 + 1] << 8 | ptr[0x13]); 
    
    for(int i = 0; i < num_of_sector; i++)
    {
        if(GetEntryVal(ptr, i) == 0x00)
        {
            return i;
        }
    }

    return -1;
}

void SetEntryVal(char *ptr, unsigned int cur_entry, unsigned int next_entry)
{
    unsigned int bytes_per_sector = (ptr[0x0B + 1] << 8 | ptr[0x0B]);

    if(cur_entry % 2 == 0)
    {
        ptr[bytes_per_sector + (3 * cur_entry) / 2 + 1] = (
            (next_entry >> 8) & 0x0F
        );
        ptr[bytes_per_sector + (3 * cur_entry) / 2] = (
            ((next_entry) & 0xFF) | (ptr[bytes_per_sector + 1 + 3 * cur_entry / 2] & 0xF0)
        );
    }
    else
    {
        ptr[bytes_per_sector + (3 * cur_entry) / 2 + 1] = (
            (next_entry >> 4) & 0xff
        );
        ptr[bytes_per_sector + (3 * cur_entry) / 2] = (
            ((next_entry << 4) & 0xF0) | (ptr[bytes_per_sector + 3 * cur_entry / 2] & 0x0F)
        );
    }
}

unsigned long int GetFreeMemorySize(char *ptr)
{
    int free_sector = 0, len = ptr[19] | (ptr[20] << 8);
    for(int i = 0; i < len; i++)
    {
        if(GetEntryVal(ptr, i) == 0x000)
            free_sector++;
    }

    return free_sector * 512;
}

char * SetDirectoryEntry(char* root, char *dir, struct stat *file, char *name, int itr_max)
{
    int i = 0;
    while(dir[0] != 0x00 && i++ < itr_max) dir += 32;
    if(i - 1 == itr_max)
    {
        fprintf(stderr, "Directory reached to its maximum file depth\n");
        return NULL;
    }

    for(i = 0; i < 8; i++)
    {
        if(*name != '.')
        {
            dir[i] = *name++;
        }
        else
        {
            dir[i] = ' ';
        }
    }

    name++;
    int len = strlen(name);
    for(i = 0; i < 3; i++)
    {
        if (len-- != 0)
        {
            dir[8 + i] = *name++;
        }
        else
        {
            dir[8 + i] = ' ';
        }
    }
    
    
    // set directory type
    dir[11] = 0x00;

    // set directory write time and date
    time_t t = file->st_ctim.tv_sec;
    struct tm *time =  localtime(&t);
    int year = ((time->tm_year - 80) << 9) & 0xFE00;
    int month = ((time->tm_mon + 1) << 5) & 0x01E0;
    int day = time->tm_mday & 0x001F;
    
    unsigned long int date = (year | month | day);
    dir[16] = date & 0xff;
    dir[17] = (date >> 8) & 0xff;

    int hour = ((time->tm_hour) << 11) & 0xFE00;
    int minute = ((time->tm_min) << 5) & 0x07E0;
    int second = (time->tm_sec) & 0x001F;

    unsigned long int time_val = (hour | minute | second);
    dir[14] = time_val & 0xff;
    dir[15] = (time_val >> 8) & 0xff;

    // set directory first logic cluster
    unsigned int entry = GetFreeEntryVal(root);
    SetEntryVal(root, entry, 0xff0);
    dir[26] = entry & 0xff;
    dir[27] = (entry & 0xff00) >> 8;

    // set directory size
    unsigned long int size = file->st_size;
    dir[31] = (size >> 24) & 0xff;
    dir[30] = (size >> 16) & 0xff;
    dir[29] = (size >> 8) & 0xff;
    dir[28] = size & 0xff;

    return dir;
}

void WriteFileToDisk(char *disk_dest, char *disk_dir_entry, char *file_src, int size)
{
    int file_size = size;
    unsigned int entry_val = (disk_dir_entry[26] & 0xff) | (disk_dir_entry[27] & 0xff) << 8;
    unsigned int next_entry; 

    do
    {   
        file_size -= 512;
        char *dest = disk_dest + (33 + entry_val - 2) * 512;
        
        if(file_size >= 0)
        {
            for(int i = 0; i < 512; i++)
                dest[i] = *file_src++;

            next_entry = GetFreeEntryVal(disk_dest);
            SetEntryVal(disk_dest, entry_val, next_entry);
        }
        else
        {
            for(int i = 0; i < file_size + 512; i++)
                dest[i] = *file_src++;
            
            SetEntryVal(disk_dest, entry_val, 0xfff);
        }

        entry_val = next_entry;

    } while (file_size > 0);   
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

    if((file_entry[11] & 0x10) == 0x10)
        return dest;
    
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

char * DirectoryExplorer(char *root, char ** dir, int len)
{
    char *res = root + 512 * 19;
    int counter = 0;
    
    for(int i = 0; i < len; i++)
    {
        while(res[0] != 0x00 && counter < 16)
        {
            if(!strcmp(GetFileName(res), dir[i])) break;
            res += 32;
        }
        
        int first_log = (res[26] & 0x00ff) | (res[27] & 0x00ff) << 8;
        res = root + ( 33 + first_log - 2) * 512;
    }

    return res;
}