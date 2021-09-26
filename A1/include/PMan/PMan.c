#include "PMan.h"

void bg_entry(char** argv, struct LinkedList *li){
	pid_t pid;
	
	pid = fork();
	if(pid == 0){
		if(execvp(argv[0], argv) < 0){
			perror("Error on execvp");
		}
		
		li = LinkedListInitializer(pid, argv[0]);
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {
		struct Node* n = NodeInitializer(pid, argv[0]);
		AddFront(li, n);
		// store information of the background child process in your data structures
	}
	else {
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
}

void bglist_entry(struct LinkedList* li) {
	PrintLinkedList(li);
}

void bgsig_entry(int pid, enum CMD_TYPE cmd_type, struct LinkedList *li) {
	
	struct Node* n = FindNode(li, pid);

	switch (cmd_type) {
		case CMD_BGKILL:
			n->state = TERM;
			break;
		case CMD_BGSTOP:
			n->state = STOP;
			break;
		case CMD_BGCONT:
			n->state = CONT;
			break;
		
		//to supress -Wall warning
		case CMD_BG:
		case CMD_BGLIST:
		case CMD_PSTAT:
		case UNRECOGNIZABLE:
			break;
	}
}

void pstat_entry(int pid, struct LinkedList *li) {
	struct Node* n = FindNode(li, pid);
	PrintNode(n);
}

void check_zombieProcess(int* headPnode){
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