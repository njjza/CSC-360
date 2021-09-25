#include "./include/PMan/PMan.h"

int pid;
char** argv;
int headPnode;

int main(){
	char* str[2];
	char* cmd;

	while(1){
		cmd = readline("PMan: > ");
		if (cmd == NULL) {
			continue;
		}

		enum CMD_TYPE cmd_type = CommandParser(cmd, str);
		continue;
		
		if (cmd_type == CMD_BG){
			printf("CMD_BG");
			//bg_entry(argv);
		}
		else if(cmd_type == CMD_BGLIST){
			printf("CMD_BGLIST");
			//bglist_entry();
		}
		else if(cmd_type == CMD_BGKILL || cmd_type == CMD_BGSTOP || cmd_type == CMD_BGCONT){
			printf("CMD_BGKILL, CMD_BGSTOP, CMD_BGCONT");
			//bgsig_entry(pid, cmd_type);
		}
		else if(cmd_type == CMD_PSTAT){
			printf("CMD_PSTAT");
			//pstat_entry(pid);
		}
		else {
			printf("Remaining Functions");
			//usage_pman();
		}
		//check_zombieProcess();
		#ifdef DEBUG
		printf("\n");
		#endif
	}

	return 0;
}