#include "Clerk.h"

struct Clerk *ClerkFactory(unsigned int id, pthread_cond_t cond)
{
    struct Clerk *clerk = malloc(sizeof(struct Clerk));
    clerk->clerk_id = id;
    clerk->convar = cond;
    clerk->condition = 0;
    return clerk;
}

void *ClerkRun(void *clerk_info)
{
    struct Clerk *clerk = (struct Clerk *)clerk_info, **server;
    struct Customer *cus_info, **winner;
    struct Queue *p_queue;
    unsigned int id = clerk->clerk_id, class;

    pthread_mutex_t *p_mutex_read;
    pthread_cond_t *p_cond;

    while (1)
    {
        if (!pthread_mutex_trylock(&queue_mutex_list[3]))
        {
            class = 1;
        }
        else if (!pthread_mutex_trylock(&queue_mutex_list[2]))
        {
            class = 0;
        }
        else
        {
            continue;
        }

        p_queue = queue_list[class];
        p_mutex_read = &queue_mutex_list[class + 2];
        p_cond = &queue_cond_list[class];
        winner = &queue_winner[class];
        server = &queue_winner_server[class];

        if ((cus_info = QueuePeek(p_queue)) == NULL)
        {
            pthread_mutex_unlock(p_mutex_read);
            continue;
        }
        clerk->condition = 0;
        *winner = cus_info;
        *server = clerk;
        
        while(cus_info == QueuePeek(p_queue))
        {
            pthread_cond_broadcast(p_cond);
        }
        
        pthread_mutex_unlock(p_mutex_read);

        fprintf(out,
                "A clerk starts serving a customer: start time %.2f, \
                the customer ID %2d, the clerk ID %1d. \n",
                (GetCurrentTime() - init_time), cus_info->user_id,
                id);

        // calculate out the waiting time of the customer
        cus_info->arrival_time = GetCurrentTime() - cus_info->arrival_time;

        usleep(cus_info->service_time);

        fprintf(out,
                "A clerk finishes serving a customer: end time %.2f, \
                the customer ID %2d, the clerk ID %1d. \n",
                (GetCurrentTime() - init_time), cus_info->user_id,
                id);
        
        pthread_mutex_lock(p_mutex_read);
        clerk->condition = 1;
        pthread_mutex_unlock(p_mutex_read);
        pthread_cond_broadcast(&clerk->convar);
    }

    return NULL;
}