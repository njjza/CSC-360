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

struct Queue * QueueFactory(void *val) {
    struct Node *n = NodeFactory(val);
    struct Queue *q = (struct Queue *) malloc(sizeof(struct Queue));
    if (!q) {
        printf("Queue Malloc Failed\n");
        exit(1);
    }

    q -> head = n;
    q -> tail = n;
    q->size = 1;

    return q;
}

void QueueInsert(void *val, struct Queue *queue) {
    struct Node *n_tail = queue -> tail;
    struct Node *n_tmp = NodeFactory(val);

    n_tail -> next = n_tmp;
    queue -> tail = n_tmp;
    queue -> size = queue -> size + 1;
}

struct Node * QueuePop(struct Queue * queue) {
    struct Node * n = queue -> head;

    queue -> head = n -> next;
    queue -> size = queue -> size - 1;

    return n;
}

struct Node * QueuePeek(struct Queue * queue) {
    struct Node * n = queue -> head;

    return n;
}