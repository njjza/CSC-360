#include "Customer.h"

struct Customer * CustomerFactory(  int id, int class, double arrival_time, 
                                    double service_time) 
{
    struct Customer *c = malloc(sizeof(struct Customer));

    c->arrival_time = arrival_time;
    c->service_time = service_time;
    c->class_type = class;
    c->user_id = id;

    return c;
}

void *CustomerRun(void *cus_info) 
{
    struct Customer *p_cus = cus_info;
    struct Queue *p_queue = queue_list[p_cus->class_type];
    pthread_mutex_t *p_queue_lock = &mutex_list[p_cus->class_type];
    pthread_cond_t *p_queue_cond = &cond_list[p_cus->class_type];

    usleep(p_cus->arrival_time);

    fprintf(out, "A customer arrives: customer ID %2d. \n", p_cus->user_id);
    
    p_cus->arrival_time = GetCurrentTime();
    
    printf("Customer ID: %d Class: %d is going to lock - start\n", 
            p_cus->user_id, p_cus->class_type);
    pthread_mutex_lock(p_queue_lock);
    printf("Customer ID: %d Class: %d is locked - start\n", 
            p_cus->user_id, p_cus->class_type);

    // fprintf(out, "A customer enters a queue: the queue ID %1d, \
    //         and length of the queue %2d. \n", 
    //         p_cus->class_type, p_queue->size+1);
    
    QueueAdd((void *) p_cus, p_queue);

    //update arrival time to enque time
    p_cus->arrival_time = GetCurrentTime();

    // customer waiting for the line.
    for(;;) {
        if (QueuePeek(p_queue) == p_cus)
        {
            printf("Customer ID: %d Class: %d is going to unlock and locked - before wait\n", 
                    p_cus->user_id, p_cus->class_type);
            pthread_mutex_unlock(p_queue_lock);
            pthread_cond_wait(p_queue_cond, p_queue_lock);
            printf("Customer ID: %d Class: %d locked - after wait\n", 
                    p_cus->user_id, p_cus->class_type);
            break;
        }
    }
    
    printf("Customer ID: %d Class: %d is going to unlock - before exit\n", 
            p_cus->user_id, p_cus->class_type);
    pthread_mutex_unlock(p_queue_lock);
    printf("Customer ID: %d Class: %d is unlocked - after exit\n\n", 
            p_cus->user_id, p_cus->class_type);
    
    pthread_exit(NULL);
}

void CustomerTest( struct Customer * cus) 
{
    printf("id: %d\t class: %d\t a.t: %ld\t p.t: %ld\n",
        cus->user_id, cus->class_type, (long int) cus->arrival_time, (long int) cus->service_time
    );
}