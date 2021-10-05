#include "./include/PMan/PMan.h"
#include "./include/LinkedList/LinkedList.h"
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

enum CMD_TYPE CommandParser(char* cmd, char*** string_vector_holder);
void FreeDArray(char ***ptr);

int main (void) {
    int pid;
    char **argv, *cmd;
    struct LinkedList *li = NULL;

	while(1){
		cmd = readline("PMan: > ");
        if (!strcmp(cmd, "")) {
            goto end;
        }

		enum CMD_TYPE cmd_type = CommandParser(cmd, &argv);

        switch (cmd_type) {
            case CMD_BG:
			    bg_entry(argv, &li);
                break;
            case CMD_BGLIST:
                bglist_entry(li);
                break;
            case CMD_BGKILL:
            case CMD_BGSTOP:
            case CMD_BGCONT:
                ; //supress error
                pid = atoi(argv[0]);
                bgsig_entry(pid, cmd_type, li);
                break;
            case CMD_PSTAT:
                pid = atoi(argv[0]);
                pstat_entry(pid);
                break;

            case UNRECOGNIZABLE:
                goto end;
                break;
        }

        FreeDArray(&argv);

        end:
        check_zombieProcess(&li);
	}

	return 0;
}

/**
 * Function Name: CommandParser
 * Input		: char* cmd, int* pid
 * Return		: enum CMD_TYPE
 * Description	: This function is used to determine the proper CMD_TYPE
 *                and pid based on the input from stdin;
**/
enum CMD_TYPE CommandParser(char* cmd, char*** string_vector_holder) {
    #define TypeDeterminer(str, type) {\
        if (!strcmp(str, "bg")) { type = CMD_BG; }\
        else if (!strcmp(str, "bgkill")) { type = CMD_BGKILL; }\
        else if (!strcmp(str, "bgstop")) { type = CMD_BGSTOP; }\
        else if (!strcmp(str, "bgstart")) { type = CMD_BGCONT; }\
        else if (!strcmp(str, "pstat")) { type = CMD_PSTAT; }\
        else if (!strcmp(str, "bglist")) { type = CMD_BGLIST; }\
        else { printf("PMan: > %s: command not found\n", str); type = UNRECOGNIZABLE; return type;}\
    }
    
    #define Realloc(ptr, size, lim) {\
        if(lim == size) {\
            size *= 2;\
            ptr = realloc(ptr, sizeof(char*) * size);\
        }\
    }

    enum CMD_TYPE type;
    char **str_holder = NULL, *str_holder_tmp, *str_tmp;
    int i = 1, size = 1;

    //retrieve and analysis command from input
    str_tmp = strtok(cmd, " ");
    TypeDeterminer(str_tmp, type);

    //check if there is more input
    str_tmp = strtok(NULL, " ");
    if(str_tmp == NULL && type != CMD_BGLIST) {goto incorrect_format;}

    str_holder = malloc(sizeof(char*) * 1);
    str_holder[0] = str_tmp;
    
    str_tmp = strtok(NULL, " ");
    while(str_tmp != NULL) {
        Realloc(str_holder, size, i);
        str_holder[i++] = str_tmp;

        str_tmp = strtok(NULL, " ");
    }

    int a = (type == CMD_BGKILL) || (type == CMD_BGSTOP) || (type == CMD_BGCONT);
    if(i > 1 && a) {goto incorrect_format;}

    Realloc(str_holder, size, i);
    str_holder[i] = NULL;

    *string_vector_holder = str_holder;
    return type;

    incorrect_format:
    printf("command is in incorrect format\n");
    
    if(str_holder != NULL) {
        FreeDArray(&str_holder);
    }
    return UNRECOGNIZABLE;
}

/**
 * Function Name: FreeDArray
 * Input		: void** ptr, size_t size
 * Return		: void
 * Description	: This function is used to free all memories that
 *                the input DArray has associated to;
**/
void FreeDArray(char ***ptr) {
    free(*ptr);
    *ptr = NULL;
}