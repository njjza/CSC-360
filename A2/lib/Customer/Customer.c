#include "Customer.h"

struct Customer * CustomerFactory(  int id, int class, double arrival_time, 
                                    double service_time) 
{
    struct Customer *c = malloc(sizeof(struct Customer));

    c->arrival_time = arrival_time;
    c->service_time = service_time;
    unsigned int *ptr = &c->class_type;
    *ptr = class;
    c->user_id = id;

    return c;
}

void *CustomerRun(void *cus_info) 
{
    struct Clerk *server;
    struct Customer *p_cus = cus_info;
    unsigned int class = p_cus->class_type;
    struct Queue *p_queue = queue_list[class];
    pthread_mutex_t *p_queue_lock = &queue_mutex_list[class];
    pthread_cond_t *p_queue_cond = &queue_cond_list[class];

    usleep(p_cus->arrival_time);
    p_cus->arrival_time = GetCurrentTime();

    fprintf(out, "A customer arrives: customer ID %2d. \n", p_cus->user_id);
    
    pthread_mutex_lock(p_queue_lock);

    
    QueueAdd((void *) p_cus, p_queue);
    fprintf(out, "A customer enters a queue: the queue ID %1d, \
            and length of the queue %2d. \n", 
            class, p_queue->size);
        
    // customer waiting for the line.
    while(queue_winner[class] != p_cus)
    {
        pthread_cond_wait(p_queue_cond, p_queue_lock);
    }

    server = queue_winner_server[class];
    QueuePop(p_queue);

    while (!server->condition)
    {
        pthread_cond_wait(&server->convar, p_queue_lock);
    }
    pthread_mutex_unlock(p_queue_lock);
    pthread_exit(NULL);
    return NULL;
}