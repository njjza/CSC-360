#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

#include "Queue.h"
struct Customer{ /// use this struct to record the customer information read from customers.txt
    int user_id;
	int class_type;
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
extern pthread_cond_t cond_list[2];
extern pthread_mutex_t mutex_list[4];
extern unsigned int queue_status[2];
extern FILE *out;

struct Customer *CustomerFactory(int id, int class, double arrival_time, double service_time);
struct CustomerThread *CustomerThreadFactory(struct Customer *customer, pthread_t id);
void * CustomerRun(void *cus_info);
void CustomerPrint( struct Customer * cus);

void CustomerTest( struct Customer * cus);
#endif