#include "Queue.h"

struct Node * NodeFactory(void *val) {
    struct Node *n = (struct Node *) malloc(sizeof(struct Node));
    if (!n) {
        printf("Node Malloc Failed\n");
        exit(1);
    }

    n -> val = val;
    n -> next = NULL;

    return n;
}

struct Queue * QueueFactory() {
    struct Queue *q = (struct Queue *) malloc(sizeof(struct Queue));
    if (!q) {
        printf("Queue Malloc Failed\n");
        exit(1);
    }

    q -> head = NULL;
    q -> tail = NULL;
    q->size = 0;

    return q;
}

void QueueAdd(void *val, struct Queue *queue) {
    struct Node *n_tail = queue -> tail;
    struct Node *n_tmp = NodeFactory(val);

    if(n_tail == NULL) {
        queue -> tail = n_tmp;
        queue -> head = n_tmp;
        goto QueueInsertBackEnd;
    }

    n_tail -> next = n_tmp;
    queue -> tail = n_tmp;

    QueueInsertBackEnd:
    queue -> size = queue -> size + 1;
}
struct Node * QueuePop(struct Queue * queue) {
    struct Node * n;
    n = queue->head;

    if (n == NULL) {
        return NULL;
    }

    if (n->next) 
    {
        queue->head = n->next;
    }
    else 
    {
        queue->head = NULL;
        queue->tail = NULL;
    }

    queue->size = queue->size - 1;

    return n;
}

struct Node * QueuePeek(struct Queue * queue) {
    return queue->head;
}