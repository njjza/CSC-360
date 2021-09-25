#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum CMD_TYPE {CMD_BG, CMD_BGCONT, CMD_BGKILL, CMD_BGLIST, CMD_BGSTOP, CMD_PSTAT, UNRECOGNIZABLE};

// provided functions - required to complete this assignment
void bg_entry(char** argv);
void bglist_entry();
void bgsig_entry(int pid, enum CMD_TYPE cmd_type);
void pstat_entry(int pid);
void check_zombieProcess(int* headPnode);