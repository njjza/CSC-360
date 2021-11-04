#include "Clerk.h"

struct Clerk * ClerkFactory(unsigned int id) {
    struct Clerk *clerk = malloc(sizeof(struct Clerk));
    clerk->clerk_id = id;
    clerk->status = IDLE;

    return clerk;
}

void *ClerkRun(void *clerk_info) {
    struct Clerk *clerk = (struct Clerk *) clerk_info;
    struct Customer *cus_info;
    struct Queue *p_queue;
    unsigned int id = clerk->clerk_id;

    pthread_mutex_t *p_mutex_read, *p_mutex_write;
    pthread_cond_t *p_cond;

    while (1) {
        if(!pthread_mutex_trylock(&mutex_list[3]))
        {
            p_mutex_read = &mutex_list[3];
            p_mutex_write = &mutex_list[1];
            p_cond = &cond_list[1];
            p_queue = queue_list[1];
        }
        else if(!pthread_mutex_trylock(&mutex_list[2]))
        {
            p_mutex_read = &mutex_list[2];
            p_mutex_write = &mutex_list[0];
            p_cond = &cond_list[0];
            p_queue = queue_list[0];
        }
        else {
            continue;
        }

        if((cus_info = QueuePeek(p_queue)) == NULL) {
            pthread_mutex_unlock(p_mutex_read);
            continue;
        }

        // wakeup correspond customer
        printf("Cluster: %d serving Costumer: %d is going to lock - before pop\n", id, cus_info->user_id);
        pthread_mutex_lock(p_mutex_write);
        printf("Cluster: %d serving Costumer: %d is locked - before pop\n", id, cus_info->user_id);
        
        pthread_cond_signal(p_cond);
        cus_info = QueuePop(p_queue);
        
        printf("Cluster: %d serving Costumer: %d is going to unlock - after pop\n", id, cus_info->user_id);
        pthread_mutex_unlock(p_mutex_write);
        printf("Cluster: %d serving Costumer: %d is unlocked - after pop\n", id, cus_info->user_id);

        pthread_mutex_unlock(p_mutex_read);


        // fprintf(out,
        //     "A clerk starts serving a customer: start time %.2f, \
        //     the customer ID %2d, the clerk ID %1d. \n", 
        //     (cus_info->arrival_time - init_time), cus_info->user_id,
        //     id
        // );

        // calculate out the waiting time of the customer
        cus_info->arrival_time = GetCurrentTime() - cus_info->arrival_time;

        usleep(1000000);

        // fprintf(out, 
        //     "A clerk finishes serving a customer: end time %.2f, \
        //     the customer ID %2d, the clerk ID %1d. \n", 
        //     (GetCurrentTime() - init_time), cus_info->user_id,
        //     id
        // );

    }

    return NULL;
}