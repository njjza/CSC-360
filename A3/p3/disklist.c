#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

enum FAT_FILE_ATTRIBUTE_MASK
{
    READ_ONLY_MASK      = 0x01,
    HIDDEN_MASK         = 0x02,
    SYSTEM_MASK         = 0x04,
    VOLUME_LABEL_MASK   = 0x08,
    SUBDIRECTORY_MASK   = 0x10,
    ARCHIVE_MASK        = 0x20
};

int bytes_per_sec;

char * PrintDirectory(char *parent_dir, char *dir_name);
int FileIterator(char* root, char *dir_file, char* dir, int max_len);

int main(int argc, char *argv[])
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
    
    bytes_per_sec = (unsigned int) (char_ptr[0x0B + 1] << 8 | char_ptr[0x0B]);   
    
    PrintDirectory("", "Root");
    FileIterator(char_ptr, char_ptr + bytes_per_sec * 19, "/", 14);

}

char * GetFileName(char * file_entry);
void print_date_time(char * directory_entry_startPos);
void PrintFile(char *file_entry, char *name, char *type, int size);

int FileIterator(char* root, char* file_dir, char *dir, int max_len)
{
    int dir_entry_value_list[max_len], counter = 0;
    char *dir_name_list[max_len];

    char * type, * file_name;
    unsigned int size, cluster_entry;
    int directory_flag, hidden_file_flag, volume_flag, dot_file_flag;
    int i = 0;
    while(file_dir[0] != 0x00 && i < max_len)
    {
        directory_flag = (file_dir[11] & SUBDIRECTORY_MASK) == SUBDIRECTORY_MASK;
        hidden_file_flag = (file_dir[11] & HIDDEN_MASK) == HIDDEN_MASK;
        volume_flag = (file_dir[11] & VOLUME_LABEL_MASK) == VOLUME_LABEL_MASK;
        dot_file_flag = file_dir[0] == '.';
        cluster_entry = (file_dir[26] & 0x00ff) | (file_dir[27] & 0x00ff) << 8;

        if(hidden_file_flag || volume_flag || dot_file_flag || cluster_entry <= 1)
            goto end_of_file_iterator_loop;

        file_name = GetFileName(file_dir);
        
        size = (
            (file_dir[28] & 0x00ff) | ((file_dir[29] & 0x00ff) << 8) | 
            ((file_dir[30] & 0x00ff) << 16) | ((file_dir[31] & 0x00ff) << 24)
        );

        if(directory_flag)
        {
            dir_entry_value_list[counter] = cluster_entry;
            dir_name_list[counter] = file_name;
            counter++;

            type = "D";
        }

        else if(!(volume_flag && dot_file_flag))
        {
            type = "F";
        }

        PrintFile(file_dir, file_name, type, size);

        end_of_file_iterator_loop:
        file_dir += 32;
    }

    for(int i = 0; i < counter; i++)
    {
        char *new_ptr = &root[(33 + dir_entry_value_list[i] - 2) * 512];
        char *new_dir = PrintDirectory(dir, dir_name_list[i]);

        FileIterator(root, new_ptr, new_dir, 16);
    }

    return 0;
}

// this method is from Week 10's material.
void print_date_time(char * directory_entry_startPos)
{
    int time, date;
    int hours, minutes, day, month, year;
    
    time = *(unsigned short *)(directory_entry_startPos + 14);
    date = *(unsigned short *)(directory_entry_startPos + 16);
    
    //the year is stored as a value since 1980
    //the year is stored in the high seven bits
    year = ((date & 0xFE00) >> 9) + 1980;
    //the month is stored in the middle four bits
    month = (date & 0x1E0) >> 5;
    //the day is stored in the low five bits
    day = (date & 0x1F);
    
    printf("%d-%02d-%02d ", year, month, day);
    //the hours are stored in the high five bits
    hours = (time & 0xF800) >> 11;
    //the minutes are stored in the middle 6 bits
    minutes = (time & 0x7E0) >> 5;
    
    printf("%02d:%02d\n", hours, minutes);
    
    return ;
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

void PrintFile(char *file_entry, char *name, char *type, int size)
{
    printf("%s %10d %20s ", type, size, name);
    print_date_time(file_entry);
}

char * PrintDirectory(char *parent_dir, char *dir_name)
{
    int len_par = strlen(parent_dir);
    int len_dir = strlen(dir_name);
    int len = len_par + len_dir;

    char *str = malloc(sizeof(char) * len + 2);
    strncat(str, parent_dir, len_par);
    strncat(str, dir_name, len_dir);
    str[len + 1] = '\0';
    
    printf("%s", str);
    printf("\n==================\n");
    str[len] = '/';
    return str;
}

