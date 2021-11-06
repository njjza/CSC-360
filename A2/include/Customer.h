#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#include "GetCurrentTime.h"
#include "Queue.h"
#include "Clerk.h"

struct Customer{ /// use this struct to record the customer information read from customers.txt
    unsigned int user_id;
	const unsigned int class_type;
	double service_time;
	double arrival_time;
};

struct CustomerThread {
	struct Customer *customer;
	pthread_t thread_id;
	pthread_cond_t condition_id;
};

extern double init_time;
extern struct Queue *queue_list[2];
extern struct Clerk *queue_winner_server[2];
extern struct Customer *queue_winner[2];
extern pthread_cond_t queue_cond_list[2], clerk_cond_list[5];
extern pthread_mutex_t queue_mutex_list[4];
extern unsigned int queue_winner_server_status[2];

extern FILE *out;

struct Customer *CustomerFactory(int id, int class, double arrival_time, double service_time);
void * CustomerRun(void *cus_info);

#endif