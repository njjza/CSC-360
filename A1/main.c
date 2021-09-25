#include "./include/PMan/PMan.h"
#include "./include/LinkedList/LinkedList.h"
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

enum CMD_TYPE CommandParser(char* cmd, char*** string_vector_holder, int *size_holder);
void FreeDArray(char ** ptr, size_t size);

int main (void) {
    char **argv, *cmd;
    int len;
    struct LinkedList head;

	while(1){
		cmd = readline("PMan: > ");
        if (!strcmp(cmd, "")) {
            continue;
        }

		enum CMD_TYPE cmd_type = CommandParser(cmd, &argv, &len);

        switch (cmd_type) {
            case CMD_BG:
			    bg_entry(argv);
                break;
            case CMD_BGLIST:
                printf("CMD_BGLIST");
                //bglist_entry();
                break;
            case CMD_BGKILL:
            case CMD_BGSTOP:
            case CMD_BGCONT:
                printf("CMD_BGKILL, CMD_BGSTOP, CMD_BGCONT");
                //bgsig_entry(pid, cmd_type);
                break;
            case CMD_PSTAT:
                printf("CMD_PSTAT");
                //pstat_entry(pid);
                break;
        }

        FreeDArray(argv, len);
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
enum CMD_TYPE CommandParser(char* cmd, char*** string_vector_holder, int *size_holder) {
    #define TypeDeterminer(str, type) {\
        if (!strcmp(str, "bg")) { type = CMD_BG; }\
        else if (!strcmp(str, "bgkill")) { type = CMD_BGKILL; }\
        else if (!strcmp(str, "bgstop")) { type = CMD_BGSTOP; }\
        else if (!strcmp(str, "bgstart")) { type = CMD_BGCONT; }\
        else if (!strcmp(str, "pstat")) { type = CMD_PSTAT; }\
        else if (!strcmp(str, "bglist")) { type = CMD_BGLIST; }\
        else { printf("PMan: > %s: command not found", str); type = UNRECOGNIZABLE; return type;}\
    }
    
    enum CMD_TYPE type;
    char **str_holder, *str_holder_tmp, *str_tmp;
    int i = 1;

    str_tmp = strtok(cmd, " ");
    TypeDeterminer(str_tmp, type);

    str_tmp = strtok(NULL, " ");
    if(str_tmp == NULL && type != CMD_BGLIST) {goto incorrect_format;}

    str_holder = malloc(sizeof(char*) * 1);
    *string_vector_holder = str_holder;
    str_holder[0] = str_tmp;

    *size_holder = 1;
    str_tmp = strtok(NULL, " ");

    while(str_tmp != NULL) {
        str_holder = realloc(str_holder, sizeof(char*) * (i + 1));

        str_holder[i++] = str_tmp;
        *string_vector_holder = str_holder;
        *size_holder = i;
        str_tmp = strtok(NULL, " ");
    }

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
void FreeDArray(char ** ptr, size_t size) {
    free(ptr);
    ptr = NULL;
}