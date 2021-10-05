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

/**
 *  Function Name   : bg_entry
 *  Input           : char** argv (a string array of the command for exec())
 *                    Linkedlist** li
 *  Return          : void
 *  Description     : Based on the argv, adding node at the back of the linkedlist;
 *                    Creating the child process and executing a file/command with 
 *                    fork() and exec();
**/
void bg_entry(char** argv, struct LinkedList **li);

/**
 *  Function Name   : bglist_entry 
 *  Input           : struct LinkedList* li
 *  Return          : void
 *  Description     : Printout all the node in the linkedlist with the format as:
 *                    <pid> : <src>/<executed file name>
**/
void bglist_entry(struct LinkedList* li);

/**
 *  Function Name   : bgsig_entry 
 *  Input           : int pid
 *                    enum CMD_TYPE cmd_type
 *                    struct LinkedList *li
 *  Return          : void
 *  Description     : Printout all the node in the linkedlist with the format as:
 *                    <pid> : <src>/<executed file name>
**/
void bgsig_entry(int pid, enum CMD_TYPE cmd_type, struct LinkedList *li);

/**
 *  Function Name   : pstat_entry
 *  Input           : int pid;
 *  Return          : void
 *  Description     : Formualte print out the /proc/id/stat 
**/
void pstat_entry(int pid);

/**
 *  Function Name   : check_zombieProcess
 *  Input           : struct LinkedList **list
 *  Return          : void
 *  Description     : Delete dead process from the linkedlist
**/
void check_zombieProcess(struct LinkedList **list);

#endif
