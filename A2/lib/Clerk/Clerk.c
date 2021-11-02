#include "Clerk.h"

struct Clerk * ClerkFactory(unsigned int id) {
    struct Clerk *clerk = malloc(sizeof(struct Clerk));
    clerk->clerk_id = id;
    clerk->status = Idle;

    return clerk;
}

void ClerkRunMain(struct Queue* q, unsigned int clerk_id) {
    int c_id;
    double time;
    struct timeval t;
    struct CustomerThread *c;
    struct Customer *cus;

    if(QueuePeek(q) == NULL) {return;}

    c = QueuePop(q)->val;
    cus = c->customer;
    c_id = cus->user_id;

    gettimeofday(&t, NULL);
    time = (t.tv_sec + (double) t.tv_usec / 1000000);

    fprintf(out, ">>>\tA clerk starts serving a customer: start time %.2f, \
            the customer ID %2d, the clerk ID %1d.\n",
            (time - init_time), c_id, clerk_id
    );
    printf(">>>\t Processing Time Required %fs\n\n", cus->service_time/10000);

    usleep(cus->service_time);
    gettimeofday(&t, NULL);

    if (pthread_cond_signal(&c->condition_id))
    {
        fprintf(stderr, "broadcast failed");
        exit(EXIT_FAILURE);
    }
    
    time = (t.tv_sec + (double) t.tv_usec / 1000000);
    fprintf(out, ">>>\tA clerk finishes serving a customer: end time %.2f, \n\
            \t\t\tthe customer ID %2d, the clerk ID %1d. \n\n",
            (time - init_time), c_id, clerk_id
    );
}

void * ClerkRun(void *clerk_info) {
    struct Clerk *clerk = (struct Clerk *) clerk_info;
    pthread_mutex_t *mus = mutex_list;
    struct Queue **qus = queue_list;

    // q_high -> high priority queus, q_low -> low priority queues;
    struct Queue *q_high = qus[1], *q_low = qus[0];
    unsigned int id = clerk->clerk_id;

    while (1) {
        // vip client 
        if (!pthread_mutex_trylock(&mus[1])) {
            ClerkRunMain(q_high, id);
            pthread_mutex_unlock(&mus[1]);
        }
        
        // regular client
        else if(!pthread_mutex_trylock(&mus[0])) {
            ClerkRunMain(q_low, id);
            pthread_mutex_unlock(&mus[0]);
        }
    }

    return NULL;
}