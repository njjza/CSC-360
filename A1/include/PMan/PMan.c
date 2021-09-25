#include "PMan.h"

/**
 * Function Name: CommandParser
 * Input		: char* cmd, char* result_buffer
 * Return		: enum CMD_TYPE
 * Description	: This function is used to print the std messages to the stdout.
 * 				  Read & Parse the input from stdin and determine the proper command
 * 				  type to return.
**/
enum CMD_TYPE CommandParser(char* cmd, char* result_buffer) {
	
	for(int i = 0; i < strlen(cmd); i++) {

	}

	return TEST;
}

void bg_entry(char **argv){
	
	pid_t pid;
	pid = fork();
	if(pid == 0){
		if(execvp(argv[0], argv) < 0){
			perror("Error on execvp");
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {
		// store information of the background child process in your data structures
	}
	else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}

void bglist_entry() {

}

void bgsig_entry(int pid, enum CMD_TYPE cmd_type) {

}

void pstat_entry(int pid) {

}

void check_zombieProcess(int headPnode){
	int status;
	int retVal = 0;
	
	while(1) {
		usleep(1000);
		if(headPnode == NULL){
			return ;
		}

		retVal = waitpid(-1, &status, WNOHANG);
		if(retVal > 0) {
			//remove the background process from your data structure
		}
		else if(retVal == 0){
			break;
		}
		else{
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
	}
	return ;
}