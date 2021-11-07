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

/**
 *  Function Name   : NodeFactory
 *  Input           : void *val
 *  Return          : struct Node *
 *  Description     : A constructor function that is to create a Node using
 *                    using malloc
**/
struct Node * NodeFactory(void *val);

/**
 *  Function Name   : QueueFactory
 *  Input           : void
 *  Return          : struct Queue *
 *  Description     : A constructor function that is to create a Queue using
 *                    using malloc
**/
struct Queue * QueueFactory();

/**
 *  Function Name   : QueueAdd
 *  Input           : void *val
 *                    struct Queue *queue
 *  Return          : void
 *  Description     : Adding an element to the back of the queue. Under the
 *                    FIFO convention, it will be the last come out of the
 *                    queue.
**/
void QueueAdd(void *val, struct Queue *queue);

/**
 *  Function Name   : QueuePop
 *  Input           : struct Queue *queue
 *  Return          : void *
 *  Description     : Retrieve the element from the queue, and throw out
 *                    The elements will be dereferenced fromt the
 *                    queue.
**/
void * QueuePop(struct Queue * queue);

/**
 *  Function Name   : QueuePeek
 *  Input           : struct Queue *queue
 *  Return          : void *
 *  Description     : Retrieve the head elements of the queue, without
 *                    causing any changes to the data structure.
**/
void * QueuePeek(struct Queue * queue);

/**
 *  Function Name   : QueuePeek
 *  Input           : struct Queue *queue
 *  Return          : int
 *  Description     : Return number of elements that the queue is currently
 *                    holding
**/
int QueueSize(struct Queue * queue);

#endif