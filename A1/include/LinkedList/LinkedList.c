#include "LinkedList.h"

struct Node *NodeInitializer(int val, char** str_vec) {
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node->next = NULL;
    node->prev = NULL;

    node->val = val;
    node->fileDirectory = str_vec;
    return node;
}

struct LinkedList* LinkedListInitializer(int val, char** str_vec)
{
    struct Node* node = NodeInitializer(val, str_vec);
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));

    list->head = node;
    list->end = node;
    return list;
}

void AddFront(struct LinkedList *list, struct Node* node)
{
    node->next = list->head;
    list->head->prev = node;
    list->head = node;
    list->size = list->size + 1;
}

void AddBack(struct LinkedList *list, struct Node* node) {
    node->prev = list->end;
    list->end->next = node;
    list->end = node;
    list->size = list->size + 1;
}

struct Node* FindNode(struct LinkedList *list, int n)
{
    struct Node* node_tmp = list->head;

    while(node_tmp != NULL) {
        if(node_tmp->val == n) {
            return node_tmp;
        }

        node_tmp = node_tmp -> next;
    }

    return NULL;
}

void PrintLinkedList(struct LinkedList *li){
    int len = li->size;
    struct Node* n = li->head;
    for(int i = 0; i < len; i++){
        PrintNode(n);
        n = n -> next;
    }
}

void PrintNode(struct Node *node) {
    printf("%d, %s, %d", node->val, node->fileDirectory[0], node->state);
}