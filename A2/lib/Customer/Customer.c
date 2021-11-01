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

struct CustomerThread *CustomerThreadFactory(struct Customer *customer, 
                                             pthread_t id)
{
    struct CustomerThread *c = malloc(sizeof(struct CustomerThread));
    pthread_cond_init(&c->condition_id, NULL); 
    c -> customer = customer;
    c -> thread_id = id;

    return c;
}

void * CustomerRun(void *cus_info) 
{
    struct timeval t;
    struct Customer *cus = cus_info;
    struct CustomerThread *thread_control = 
        CustomerThreadFactory(cus, pthread_self());
    struct Queue *q = queue_list[cus->class_type];
    pthread_mutex_t q_lock = mutex_list[cus->class_type];

    usleep(cus->arrival_time);
    
    printf("A customer arrives: customer ID %2d. \n", cus->user_id);

    pthread_mutex_lock(&q_lock);
    printf("A customer enters a queue: the queue ID %1d, \
            and length of the queue %2d. \n", 
            cus->class_type, q->size+1);
    
    //update arrival time to enque time
    gettimeofday(&t, NULL);
    cus->arrival_time = (t.tv_sec + (double) t.tv_usec / 1000000) - init_time;
    
    QueueAdd((void *) thread_control, q);
    pthread_mutex_unlock(&q_lock);

    pthread_mutex_t lock;
    if (pthread_mutex_init(&lock, NULL) != 0){ //mutex initialization
        printf("mutex init failed\n");
        exit(EXIT_FAILURE);
    }

    pthread_cond_wait(&thread_control->condition_id, &lock);
    printf("Client: %d finished\n", cus->user_id);
    pthread_mutex_unlock(&lock);
    
    return NULL;
}

void CustomerTest( struct Customer * cus) 
{
    printf("id: %d\t class: %d\t a.t: %ld\t p.t: %ld\n",
        cus->user_id, cus->class_type, (long int) cus->arrival_time, (long int) cus->service_time
    );
}