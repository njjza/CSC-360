#include "LinkedList.h"

struct Node *NodeInitializer(int val, char *f) {
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node->next = NULL;
    node->prev = NULL;
    node->val = val;
    node->name = f;

    return node;
}
struct LinkedList* LinkedListInitializer(int val, char *f)
{
    struct Node* node = NodeInitializer(val, f);
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));

    list->head = node;
    list->end = node;
    list->size = 1;
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

void DeleteNode(struct Node *n, struct LinkedList **li) {
    struct LinkedList *li_tmp = *li;

    li_tmp->size = li_tmp->size - 1;
    if(li_tmp->size == 0) {
        free(n);
        free(*li);
        *li = NULL;
        return;
    }

    if(li_tmp->head == n) {
        li_tmp->head = n->next;
    }
    
    if(n->next != NULL) {
        n->next->prev = n->prev;
    }

    if(n->prev != NULL) {
        n->prev->next = n->next;
    }
    
    free(n);
}

void PrintLinkedList(struct LinkedList *li){
    if(li == NULL) {
        printf("PMan: > Empty Process List\n");
        return;
    }

    int len = li->size;
    struct Node* n = li->head;
    for (int i = 0; i < len; i++) {
        PrintNode(n);
        n = n -> next;
    }
}

void PrintNode(struct Node *node) {
    int pid = node->val;
    ssize_t len, alloclen = 128;
    char path[MAX_INT_STR_LENGTH], *cwd;
    
    sprintf(path, "/proc/%d/cwd", pid);
    
    cwd = malloc(alloclen);
    while((len = readlink(path, cwd, alloclen)) == alloclen) {
        alloclen *= 2;
        cwd = realloc(cwd, alloclen);
    }
    cwd[len] = '\0';

    printf("%d: %s", pid, (char *) cwd);
    printf("%s\n", node->name);
    free(cwd);
}