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
struct Clerk *queue_winner_server[2];
unsigned int queue_winner_server_status[2] = {0, 0};

struct Customer *queue_winner[2];
pthread_cond_t queue_cond_list[2], clerk_cond_list[5];
pthread_mutex_t queue_mutex_list[4];

FILE *out;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "The number of arguments is incorrect\n");
		exit(EXIT_FAILURE);
	}
	
	// initialize queues, and its mutexes, conditions
	for(int i = 0; i < 2; i++)
	{
		pthread_cond_init(&queue_cond_list[i], NULL);
		pthread_mutex_init(&queue_mutex_list[i], NULL);
		pthread_mutex_init(&queue_mutex_list[i+2], NULL);
		queue_list[i] = QueueFactory();
	}

	// initialize output stream
	out = stdout;
	// out = fopen("./output/output.txt", "w");

	// create customer list
	struct Customer **cus_list;
	int cus_pool_size = FileParser(argv[1], &cus_list);

	// create clerk list
	struct Clerk *clerk_list[5];
	for(int i = 0; i < 5; i++) {
		pthread_cond_init(&clerk_cond_list[i], NULL);
		clerk_list[i] = ClerkFactory(i + 1, clerk_cond_list[i]);
	}

	// create clerk thread
	pthread_t clerk_pool[5];
	for(int i = 0; i < 5; i++)
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

	// calculate and printout average customer's watiing time.
	double t_all = 0, t_vip = 0, t_econ = 0;
	int n_econ = 0, n_vip = 0;
	struct Customer * c;
	for (int i = 0; i < cus_pool_size; i++)
	{
		c = cus_list[i];
		t_all	+=	c->arrival_time;
		if (c->class_type == 0)
		{
			t_econ += c->arrival_time;
			n_econ++;
		}
		else
		{
			t_vip += c->arrival_time;
			n_vip++;
		}
	}

	t_all /= cus_pool_size;
	t_vip /= n_vip;
	t_econ /= n_econ;

	fprintf(out, "The average waiting time for all customers in the system is: %.2f seconds. \n", t_all);
	fprintf(out, "The average waiting time for business-class customers in the system is: %.2f seconds. \n", t_vip);
	fprintf(out, "The average waiting time for economic-class customers in the system is: %.2f seconds. \n", t_econ);

	// destroy all mutexes, convar, malloced ds
	free(queue_list[0]);
	free(queue_winner_server[0]);
	free(queue_winner[0]);

	for(int i = 0; i < 2; i++)
	{
		pthread_cond_destroy(&queue_cond_list[i]);
		pthread_cond_destroy(&clerk_cond_list[i]);
		pthread_cond_destroy(&clerk_cond_list[i + 2]);
		pthread_mutex_destroy(&queue_mutex_list[i]); 
		pthread_mutex_destroy(&queue_mutex_list[i + 2]);
	}

	pthread_cond_destroy(&clerk_cond_list[4]);

	fclose(out);
	exit(EXIT_SUCCESS);
}

/**
 * Function Name: File Parser
 * Input		: char *filepath, struct Customer ***result
 * Return		: int
 * Description	: This function is to create customers based on the input
 * 				  from argv. The result in the parameter are used to carry
 * 				  newly created customer list. The function will have
 *                the size of the customer returned in integer.
**/
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

	if(fgets(buffer, 256, f) == NULL)
	{
		fprintf(stderr, "fgets error");
		exit(EXIT_FAILURE);
	}
	len = atoi(buffer);

	for (i = 0; i < len; i++)
	{
		if (fgets(buffer, 256, f) == NULL)
		{
			fprintf(stderr, "fgets error");
			exit(EXIT_FAILURE);
		}
		
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

/**
 * Function Name: DynamicArrayAdd
 * Input		: void **pArray, void *val, int *size, int index
 * Return		: void **
 * Description	: This function is to add an element to an array
 * 				  with dynamic memory reallocation.
**/
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