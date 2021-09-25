#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/types.h>

enum CMD_TYPE {CMD_BG, CMD_BGCONT, CMD_BGKILL, CMD_BGLIST, CMD_BGSTOP, CMD_PSTAT, TEST};

// provided functions - required to complete this assignment
void bg_entry(char **argv);
void bglist_entry();
void bgsig_entry(int pid, enum CMD_TYPE cmd_type);
void pstat_entry(int pid);
void check_zombieProcess(int headPnode);

enum CMD_TYPE CommandParser(char* cmd, char* result_buffer);
