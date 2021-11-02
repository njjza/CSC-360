#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#include "./include/Queue.h"
#include "./include/Customer.h"
#include "./include/Clerk.h"

struct Customer ** FileParser(char* filepath);
void ** DynamicArrayAdd(void **pArray, void *val, int *size, int index);

int ClientNum;
double init_time;
struct Queue **queue_list; // for external file usage;
pthread_mutex_t *mutex_list; // for external file usage;
FILE * out;

int main(int argc, char *argv[]) {
	char *file_path;
	int i;
	pthread_t id;
	struct Customer **CustomerPool, *customer;
	struct Clerk** clerk_pool;
	struct Queue* queue_pool[2] = {QueueFactory(), QueueFactory()};
	pthread_mutex_t mutex_pool[4];

	queue_list = queue_pool;
	mutex_list = mutex_pool;

	if(argc < 2) {
		fprintf(stderr, "Incorrect input format, please attach input file link\n");
		exit(EXIT_FAILURE);	
	}
	
	// out = fopen("./output/output.txt", "w");
	out = stdout;
	// initialize mutex for queues. 
	// mutex_list [1] is for queue regular write
	// mutex_list [2] is for queue vip write
	// mutex_list [3] is for queue regular read
	// mutex_list [4] is for queue vip write
	for (int i = 0; i < 4; i++) {
		pthread_mutex_t mutex_id;

		if(pthread_mutex_init(&mutex_id, NULL)) {
			fprintf(stderr, "mutex initialized failed\n");
		}

		mutex_pool[i] = mutex_id;
	}

	// create clerk thread
	clerk_pool = (struct Clerk **) malloc(sizeof(void *) * 5);
	for (int i = 0; i < 5; i++) {
		struct Clerk *c = ClerkFactory(i+1);

		if(pthread_create(&id, NULL, &ClerkRun, c)){
			fprintf(stderr, "clerk thread creation failed, exit now\n");
			exit(EXIT_FAILURE);
		}
		
		clerk_pool[i] = c;
	}

	//create customer thread
	CustomerPool = FileParser(argv[1]);
	pthread_t id_list[ClientNum];
	
	struct timeval t;
	gettimeofday(&t, NULL);
	init_time = (t.tv_sec + (double) t.tv_usec / 1000000);

	i ^= i;
	while((customer = CustomerPool[i++]) != NULL) {
		if(pthread_create(&id, NULL, &CustomerRun, customer)){
			fprintf(stderr, "customer thread creation failed, exit now\n");
			exit(EXIT_FAILURE);
		}

		id_list[i-1] = id;
	}
	
	//prevent pre-mature termination
	for(i ^= i; i < ClientNum; i++) {
		if(pthread_join(id_list[i], NULL)){
			fprintf(stderr, "thread creation failed, exit now\n");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

struct Customer ** FileParser(char* filepath) {
	FILE *f = fopen(filepath, "r");
	if (f == NULL) {
		printf("incorrect file name\n");
		exit(EXIT_FAILURE);
	}
	
	int i;
	char buffer[256];
	struct Customer **li = malloc(sizeof(struct Customer *) * 5);
	struct Customer *customer;
	int id, type;
	long int arrival_time, process_time;
	int len, li_size = 5;

	fgets(buffer, 256, f);	
	len = atoi(buffer);
	ClientNum = len;

	for (i = 0; i < len; i++) {
		fgets(buffer, 256, f);
		sscanf(	buffer, "%d:%d,%ld,%ld", 
				&id, &type, &arrival_time, &process_time);
		customer = CustomerFactory(	id, type, 
									(double) arrival_time * 10000, 
									(double) process_time * 10000);
		
		
		li = (struct Customer **) DynamicArrayAdd(	(void **) li, 
													(void *) customer, 
								 					&li_size, i);
	}
	
	li = (struct Customer **) DynamicArrayAdd((void **)li, NULL, 
												&li_size, i);

	fclose(f);

	return li;
}

void ** DynamicArrayAdd(void **pArray, void *val, int *size, int index) {
	if(index >= *size) {
		pArray = realloc(pArray, index * 2 * sizeof(struct Customer *));
		if(pArray == NULL) {
			printf("pArray reallocation failed\n");
			exit(EXIT_FAILURE);
		}

		*size = index * 2;
	}
	
	pArray[index] = val;
	return pArray;
}