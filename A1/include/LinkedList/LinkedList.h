#ifndef LinkedList_H
#define LinkedList_H

#include <stdio.h>
#include <stdlib.h>

enum NodeState {TERM, STOP, CONT, ZOMBIE};

struct Node
{
    int val;
    char** fileDirectory;
    enum NodeState state;
    struct Node *next, *prev;
};

struct LinkedList
{
    struct Node *head, *end;
    int size;
};

struct LinkedList* LinkedListInitializer(int val, char** str_vec);
void AddFront(struct LinkedList *list, struct Node* node);
void AddBack(struct LinkedList *list, struct Node* node);
void PrintLinkedList(struct LinkedList *li);

struct Node* FindNode(struct LinkedList *list, int val);
struct Node *NodeInitializer(int val, char** str_vec);
void PrintNode(struct Node *node);

#endif
