#ifndef LinkedList_H
#define LinkedList_H

#define MAX_INT_STR_LENGTH 21

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

enum NodeState {TERM, STOP, CONT, ZOMBIE};

struct Node
{
    int val; //pid
    char *name;
    struct Node *next, *prev;
};

struct LinkedList
{
    struct Node *head, *end;
    unsigned int size;
};

struct LinkedList* LinkedListInitializer(int val, char *f);
void AddFront(struct LinkedList *list, struct Node* node);
void AddBack(struct LinkedList *list, struct Node* node);
void DeleteNode(struct Node* n, struct LinkedList **li);
void PrintLinkedList(struct LinkedList *li);

struct Node* FindNode(struct LinkedList *list, int val);
struct Node *NodeInitializer(int val, char *f);
void PrintNode(struct Node *node);

#endif
