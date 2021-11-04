#ifndef CLERK_H
#define CLERK_H

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "../Customer/Customer.h"
#include "../Queue/Queue.h"
#include "../CurrentTime/GetCurrentTime.h"

typedef enum {IDLE, BUSY} ClerkStatus;

struct Clerk {
    unsigned int clerk_id;
    ClerkStatus status;
    pthread_t thread_id;
};

extern double init_time;
extern struct Queue *queue_list[2];
extern pthread_cond_t cond_list[2];
extern pthread_mutex_t mutex_list[4];
extern unsigned int queue_status[2];
extern FILE *out;

struct Clerk * ClerkFactory(unsigned int id);
void * ClerkRun(void *clerk_info);

#endif