#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

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
struct Queue * QueueFactory();

void QueueAdd(void *val, struct Queue *queue);
void * QueuePop(struct Queue * queue);
void * QueuePeek(struct Queue * queue);
int QueueSize(struct Queue * queue);
#endif