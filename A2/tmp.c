#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>

#include "Queue.h"

#define NCLERKS 5
#define NQUEUE 2
#define TRUE 1
#define FALSE 0
struct Customer
{ /// use this struct to record the customer information read from customers.txt
	int user_id;
	int class_type;
	double service_time;
	double arrival_time;
};

/* global variables */
double init_time;			 // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
double overall_waiting_time = 0.0; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.

struct Queue *queue_list[2];
int queue_status[NQUEUE];		// variable to record the status of a queue, the value could be idle (not using by any clerk) or the clerk id (1 ~ 4), indicating that the corresponding clerk is now signaling this queue.
sem_t winner_selected[NQUEUE]; // variable to record if the first customer in a queue has been successfully selected and left the queue.

pthread_cond_t cond_clerkcs[5];
pthread_cond_t cond_customer_type[2];

pthread_mutex_t mutex_queue_write[2];
pthread_mutex_t mutex_queue_read[2];
pthread_mutex_t mutex_void;

void *customer_entry(void *cus_info);
void *clerk_entry(void *clerkNum);
int FileParser(char *filepath, struct Customer ***result);
void ** DynamicArrayAdd(void **pArray, void *val, int *size, int index);

int main(int argc, char *argv[])
{
	int i;
	struct timeval time;

	if (argc < 2)
	{
		exit(EXIT_FAILURE);
	}

	// initialize all the condition variable and thread lock will be used
	for(i = 0; i < 2; i++) {
		pthread_mutex_init(&mutex_queue_write[i], NULL);
		pthread_mutex_init(&mutex_queue_read[i], NULL);
		pthread_cond_init(&cond_customer_type[i], NULL);
		sem_init(&winner_selected[i], 0, 0);
		queue_list[i] = QueueFactory();
	}

	// create clercks thread
	int clerk_list[NCLERKS];
	pthread_cond_t tmp_cond;
	pthread_t tmp_thread;
	for (i = 0; i < NCLERKS; i++)
	{
		clerk_list[i] = i + 1;
		pthread_create(&tmp_thread, NULL, clerk_entry, (void *)&clerk_list[i]);
		pthread_cond_init(&tmp_cond, NULL);
		cond_clerkcs[i] = tmp_cond;
	}

	//set initial time
	gettimeofday(&time, NULL);
	init_time = (time.tv_sec + (double)time.tv_usec / 1000000);

	//create customer thread
	struct Customer **customer_list;
	int customer_length = FileParser("./input/input0.txt", &customer_list);
	// int customer_length = FileParser(argv[1], &customer_list);
	pthread_t customer_id_list[customer_length];

	for (i = 0; i < customer_length; i++)
	{
		pthread_create(&customer_id_list[i], NULL, customer_entry, (void *)customer_list[i]);
	}

	// wait for all customer threads to terminate
	for (i = 0; i < customer_length; i++)
	{
		pthread_join(customer_id_list[i], NULL);
	}

	// destroy mutex & condition variable (optional)
	pthread_mutex_destroy(&mutex_queue_write[0]);
	pthread_mutex_destroy(&mutex_queue_write[1]);
	pthread_mutex_destroy(&mutex_queue_read[0]);
	pthread_mutex_destroy(&mutex_queue_read[1]);

	pthread_cond_destroy(&cond_customer_type[0]);
	pthread_cond_destroy(&cond_customer_type[1]);

	// calculate the average waiting time of all customers
	return 0;
}



// function entry for customer threads
void *customer_entry(void *cus_info)
{
	struct timeval time;
	struct Customer *p_my_info = (struct Customer *)cus_info;
	struct Queue *p_my_queue = queue_list[p_my_info->class_type];
	pthread_mutex_t *my_mutex = &mutex_queue_write[p_my_info->class_type];
	pthread_cond_t *my_cond = &cond_customer_type[p_my_info->class_type];
	
	usleep(p_my_info->arrival_time);

	fprintf(stdout, "A customer arrives: customer ID %2d. \n", p_my_info->user_id);

	pthread_mutex_lock(my_mutex);
	{
		QueueAdd(p_my_info, p_my_queue);
		fprintf(stdout,
				"A customer enters a queue: the queue ID %1d, \
				and length of the queue %2d. \n",
				p_my_info->class_type, p_my_queue->size);
	}

	pthread_cond_wait(my_cond, my_mutex);
	
	if (p_my_info == QueuePeek(p_my_queue)->val)	
	{
		sem_wait(&winner_selected[p_my_info->class_type]);
		QueuePop(p_my_queue);
	}

	pthread_mutex_unlock(my_mutex); //unlock mutex_lock such that other customers can enter into the queue

	/* Try to figure out which clerk awoken me, because you need to print the clerk Id information */
	usleep(10);
	int clerk_woke_me_up = queue_status[p_my_info->class_type];

	/* get the current machine time; updates the overall_waiting_time*/
	gettimeofday(&time, NULL);
	p_my_info->arrival_time = (time.tv_sec + (double)time.tv_usec / 1000000);
	fprintf(stdout, 
			"A clerk starts serving a customer: start time %.2f, \
			the customer ID %2d, the clerk ID %1d. \n", 
			(p_my_info->arrival_time - init_time), p_my_info->user_id,
			clerk_woke_me_up
		);

	// processing time for a client
	usleep(p_my_info->service_time);

	/* get the current machine time; */
	gettimeofday(&time, NULL);
	p_my_info->service_time = (time.tv_sec + (double)time.tv_usec / 1000000);

	fprintf(stdout, 
			"A clerk finishes serving a customer: end time %.2f, \
			the customer ID %2d, the clerk ID %1d. \n", 
			(p_my_info->service_time - init_time), p_my_info->user_id,
			clerk_woke_me_up
		);
	
	pthread_cond_signal(&cond_clerkcs[clerk_woke_me_up]); // Notify the clerk that service is finished, it can serve another customer
	
	pthread_exit(NULL);
}

void clerk_main(int queue_id, int clerk_id) {
	queue_status[queue_id] = clerk_id;

	pthread_mutex_lock(&mutex_queue_write[queue_id]);
	if(pthread_cond_broadcast(&cond_customer_type[queue_id])) 
	{
		printf("broadcast err\n");
		exit(EXIT_FAILURE);
	}
	
	sem_post(&winner_selected[queue_id]);
	pthread_cond_wait(&cond_clerkcs[clerk_id], &mutex_void);
	pthread_mutex_unlock(&mutex_queue_write[queue_id]);
}

// function entry for clerk threads
void *clerk_entry(void *clerkNum)
{
	int selected_queue_id;
	
	while (1)
	{
		if (!pthread_mutex_trylock(&mutex_queue_read[1]))
		{
			if (QueuePeek(queue_list[1]) == NULL)
			{
				goto CLERK_VIP_END;
			}

			clerk_main(1, * (int *) clerkNum);
			CLERK_VIP_END:
			pthread_mutex_unlock(&mutex_queue_read[1]);
		}

		else if (!pthread_mutex_trylock(&mutex_queue_read[0]))
		{
			if (QueuePeek(queue_list[0]) == NULL)
			{
				goto CLERK_ECON_END;
			}

			clerk_main(0, * (int *) clerkNum);
			CLERK_ECON_END:
			pthread_mutex_unlock(&mutex_queue_read[0]);
		}
	}

	pthread_exit(NULL);
}

int FileParser(char *filepath, struct Customer ***result)
{
	FILE *f = fopen(filepath, "r");
	if (f == NULL)
	{
		printf("incorrect file name\n");
		exit(EXIT_FAILURE);
	}

	int i;
	char buffer[256];
	struct Customer **li = malloc(sizeof(struct Customer *) * 5);
	struct Customer *customer;
	int id, type;
	double arrival_time, process_time;
	int len, li_size = 5;

	fgets(buffer, 256, f);
	len = atoi(buffer);

	for (i = 0; i < len; i++)
	{
		fgets(buffer, 256, f);
		sscanf(buffer, "%d:%d,%lf,%lf",
			   &id, &type, &arrival_time, &process_time);
		
		customer = malloc(sizeof(struct Customer));
		customer->arrival_time = arrival_time * 10000;
		customer->service_time = process_time * 10000;
		customer->user_id = id;
		customer->class_type = type;

		li = (struct Customer **)DynamicArrayAdd((void **)li,
												 (void *)customer,
												 &li_size, i);
	}

	li = (struct Customer **)DynamicArrayAdd((void **)li, NULL,
											 &li_size, i);

	*result = li;

	fclose(f);

	return len;
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