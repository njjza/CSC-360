#ifndef CLERK_H
#define CLERK_H

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "Customer.h"
#include "Queue.h"

struct Clerk {
    unsigned int clerk_id;
    pthread_cond_t status;
};

extern double init_time;
extern struct Queue *queue_list[2];
extern struct Clerk *queue_winner_server[2];
extern struct Customer *queue_winner[2];
extern pthread_cond_t queue_cond_list[2], clerk_cond_list[5];
extern pthread_mutex_t queue_mutex_list[4];
extern FILE *out;

struct Clerk * ClerkFactory(unsigned int id, pthread_cond_t cond);
void * ClerkRun(void *clerk_info);

#endif