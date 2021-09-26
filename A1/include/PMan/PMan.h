#ifndef PMAN_H
#define PMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../LinkedList/LinkedList.h"

enum CMD_TYPE {CMD_BG, CMD_BGCONT, CMD_BGKILL, CMD_BGLIST, CMD_BGSTOP, CMD_PSTAT, UNRECOGNIZABLE};

// provided functions - required to complete this assignment
void bg_entry(char** argv, struct LinkedList *li);
void bglist_entry();
void bgsig_entry(int pid, enum CMD_TYPE cmd_type, struct LinkedList *li);
void pstat_entry(int pid, struct LinkedList *li);
void check_zombieProcess(int* headPnode);

#endif
