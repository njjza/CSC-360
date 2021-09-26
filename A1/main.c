#include "./include/PMan/PMan.h"
#include "./include/LinkedList/LinkedList.h"
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum CMD_TYPE CommandParser(char* cmd, char*** string_vector_holder);
void FreeDArray(char ** ptr);

int main (void) {
    char **argv, *cmd;
    struct LinkedList *li = NULL;

	while(1){
		cmd = readline("PMan: > ");
        if (!strcmp(cmd, "")) {
            continue;
        }

		enum CMD_TYPE cmd_type = CommandParser(cmd, &argv);

        switch (cmd_type) {
            case CMD_BG:
                printf("CMD_BG\n");
			    bg_entry(argv, &li);
                break;
            case CMD_BGLIST:
                printf("CMD_BGLIST");
                bglist_entry();
                break;
            case CMD_BGKILL:
            case CMD_BGSTOP:
            case CMD_BGCONT:
                printf("CMD_BGKILL, CMD_BGSTOP, CMD_BGCONT");
                int pid = atoi(argv[1]);
                bgsig_entry(pid, cmd_type, li);
                break;
            case CMD_PSTAT:
                printf("CMD_PSTAT");
                pstat_entry(pid, li);
                break;
        }

        /*
        if (li != NULL) {
            check_zombieProcess(li->head);
        }
        */
		printf("\n");
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
        else { printf("PMan: > %s: command not found", str); type = UNRECOGNIZABLE; return type;}\
    }
    
    #define Realloc(ptr, size, lim) {\
        if(lim == size) {\
            ptr = realloc(ptr, sizeof(char*) * (size * 2));\
            size *= 2;\
        }\
    }
    enum CMD_TYPE type;
    char **str_holder, *str_holder_tmp, *str_tmp;
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
    str_holder[i++] = NULL;

    *string_vector_holder = str_holder;
    return type;

    incorrect_format:
    printf("command is in incorrect format");
    return type;
    // a long list of if-else branches crumpled together to save space.
}

/**
 * Function Name: FreeDArray
 * Input		: void** ptr, size_t size
 * Return		: void
 * Description	: This function is used to free all memories that
 *                the input DArray has associated to;
**/
void FreeDArray(char ** ptr) {
    free(ptr);
    ptr = NULL;
}