#include "PMan.h"

void bg_entry(char** argv, struct LinkedList **li){
	pid_t pid;
	char *tmp = (char *) malloc(strlen(&argv[0][1]));
	strcpy(tmp, &argv[0][1]);

	pid = fork();
	if(pid == 0){
		if(execvp(argv[0], argv) < 0){
			perror("Error on execvp");
		}
		printf("PMan: > ");
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0) {
		if(*li == NULL) {
			*li = LinkedListInitializer(pid, tmp);
			return;
		}
		
		struct Node* n = NodeInitializer(pid, tmp);
		AddBack(*li, n);
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
	if(li == NULL) {printf("invalid pid\n"); return;}
	struct Node* n = FindNode(li, pid);
	if (n == NULL) {
		printf("invalid pid\n");
		return;
	}

	int ret;
	switch (cmd_type) {
		case CMD_BGKILL:
			ret = kill((pid_t) n->val, SIGTERM);
			if(ret == -1) {
				printf("kill failed ");
				exit(EXIT_FAILURE);
			}
			break;
			

		case CMD_BGSTOP:
			ret = kill((pid_t) n->val, SIGSTOP);
			if(ret == -1) {
				printf("terminate failed");
				exit(EXIT_FAILURE);
			}
			break;

		case CMD_BGCONT:
			ret = kill((pid_t) n->val, SIGCONT);
			if(ret == -1) {
				printf("continue failed");
				exit(EXIT_FAILURE);
			}
			break;

		//to supress -Wall warning
		case CMD_BG:
		case CMD_BGLIST:
		case CMD_PSTAT:
		case UNRECOGNIZABLE:
			break;
	}
}

void pstat_entry(int pid) {
	char file_name[100];
	sprintf(file_name, "/proc/%d/stat", pid);

	FILE *f = fopen(file_name, "r");
	if (f == NULL) {
		printf("process %d does not exist\n", pid);
		return;
	}

	char comm[100], state;
	int unused, ppid, pgroup, session, tty_nr, tpgid, vol, nonvol;
	long int cutime, cstime, priority, nice, num_threads, itrealvalue, rss;
	unsigned int flags;
	unsigned long int minflt, cminflt, mafflt, cmajflt, vsize;
	unsigned long long int starttime;
	float utime, stime;

	fscanf(
		f, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %f %f %ld %ld %ld %ld %ld %ld %llu, %lu %ld",
		&unused, comm, &state, &ppid,&pgroup,&session,&tty_nr,&tpgid,&flags,
		&minflt,&cminflt,&mafflt,&cmajflt,&utime,&stime, &cutime,&cstime,
		&priority,&nice,&num_threads,&itrealvalue,&starttime,&vsize,&rss
	);
	fscanf(f, "voluntary_ctxt_switches:%d",&vol);
    fscanf(f, "nonvoluntary_ctxt_switches:%d", &nonvol);
	
	printf("comm = %s,\tstate = %c,\t\t\tutime is %f,\tstime is %f\n", comm, state, utime, stime);
	printf("rss is %ld,\tvoluntary_ctxt_switches:%d\tnonvoluntary_ctxt_switches:%d\n", rss, vol, nonvol);
	fclose(f);
}

void check_zombieProcess(struct LinkedList **list){
	int status, retVal = 0;
	struct Node *headPnode, *headPnode_tmp;
	usleep(2000);

	if (list == NULL) {return;}
	headPnode = (*list)->head;

	while(headPnode != NULL) {
		headPnode_tmp = headPnode->next;

		retVal = waitpid(-1, &status, WNOHANG);
		if(retVal > 0) {
			//remove your background process from datastructure
			DeleteNode(headPnode, list);
		}
		else if(retVal == 0){
			break;
		} 
		else{
			perror("waitpid failed");
			exit(EXIT_FAILURE);
		}
		headPnode = headPnode_tmp;
	}
	return ;
}