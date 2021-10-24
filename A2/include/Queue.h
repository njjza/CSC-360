#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

struct Node {
    void *val;
    struct Node *next;
};

struct Queue {
    int size;
    struct Node *head;
    struct Node *tail;
};

struct Node * NodeFactory(void *val);
struct Queue * QueueFactory(void *val);

void QueueInsert(void *val, struct Queue *queue);
struct Node * QueuePop(struct Queue * queue);
struct Node * QueuePeek(struct Queue * queue);

#endif