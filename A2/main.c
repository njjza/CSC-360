#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>

#include "./include/Queue.h"
#include "./include/Customer.h"
#include "./include/Clerk.h"
#include "./include/GetCurrentTime.h"

int FileParser(char *filepath, struct Customer ***result);
void **DynamicArrayAdd(void **pArray, void *val, int *size, int index);

// global variables shared across multiple file
double init_time;
struct Queue *queue_list[2];
pthread_cond_t cond_list[2];
pthread_mutex_t mutex_list[4];
unsigned int queue_status[2];
FILE *out;						 

int main(int argc, char *argv[])
{
	if (argc < 2) 
	{
		fprintf(stderr, "The number of arguments is incorrect\n");
		exit(EXIT_FAILURE);
	}

	// initialize mutexes, conditions, queues, output stream
	for(int i = 0; i < 2; i++)
	{
		pthread_cond_init(&cond_list[i], NULL);
		pthread_mutex_init(&mutex_list[i], NULL);
		pthread_mutex_init(&mutex_list[i+2], NULL);
		queue_list[i] = QueueFactory();
	}

	out = fopen("./output/output.txt", "w");

	// create customer list
	struct Customer **cus_list;
	int cus_pool_size = FileParser(argv[1], &cus_list);

	// create clerk list
	struct Clerk *clerk_list[5];
	for(int i = 0; i < 5; i++) {
		clerk_list[i] = ClerkFactory(i + 1);
	}

	// create clerk thread
	pthread_t clerk_pool[5];
	for(int i = 0; i < 2; i++)
	{
		pthread_create(&clerk_pool[i], NULL, ClerkRun, clerk_list[i]);
	}

	// start clock - for precision right before customer thread creation.
	init_time = GetCurrentTime();

	// create customer thread
	pthread_t cus_pool[cus_pool_size];
	for(int i = 0; i < cus_pool_size; i++)
	{
		pthread_create(&cus_pool[i], NULL, CustomerRun, (void *) cus_list[i]);
	}

	// prevent premature termination
	for(int i = 0; i < cus_pool_size; i++)
	{
		if(pthread_join(cus_pool[i], NULL))
		{
			fprintf(stderr, "Thread Join error\n");	
		};
	}

	fclose(out);
	exit(EXIT_SUCCESS);
}

int FileParser(char *filepath, struct Customer ***result)
{
	int i, id, type, len, li_size = 5;
	long int arrival_time, process_time;
	char buffer[256];
	void **li = malloc(sizeof(struct Customer *) * li_size);
	struct Customer *customer;

	FILE *f = fopen(filepath, "r");
	if (f == NULL)
	{
		fprintf(stderr, "Incorrect file path\n");
		exit(EXIT_FAILURE);
	}

	fgets(buffer, 256, f);
	len = atoi(buffer);

	for (i = 0; i < len; i++)
	{
		fgets(buffer, 256, f);
		sscanf(buffer, "%d:%d,%ld,%ld", &id, &type, &arrival_time, 
			   &process_time);
		
		customer = CustomerFactory(id, type,
								   (double)arrival_time * 100000,
								   (double)process_time * 100000);

		li = DynamicArrayAdd(li, (void *)customer, &li_size, i);
	}

	fclose(f);

	li = DynamicArrayAdd(li, NULL, &li_size, i);

	*result = (struct Customer **) li;
	return len;
}

void **DynamicArrayAdd(void **pArray, void *val, int *size, int index)
{
	if (index >= *size)
	{
		pArray = realloc(pArray, index * 2 * sizeof(struct Customer *));
		if (pArray == NULL)
		{
			fprintf(stderr, "pArray reallocation failed\n");
			exit(EXIT_FAILURE);
		}

		*size = index * 2;
	}

	pArray[index] = val;
	return pArray;
}