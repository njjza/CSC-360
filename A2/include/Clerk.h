#ifndef CLERK_H
#define CLERK_H

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "Customer.h"
#include "Queue.h"

typedef enum {Idle, Busy} ClerkStatus;

struct Clerk {
    unsigned int clerk_id;
    ClerkStatus status;
    pthread_t thread_id;
};

extern double init_time;
extern struct Queue **queue_list;
extern pthread_mutex_t *mutex_list;
extern FILE* out;

struct Clerk * ClerkFactory(unsigned int id);
void * ClerkRun(void *clerk_info);

#endif