#include "LinkedList.h"

struct Node *NodeInitializer(int val, char *f) {
    if(f == NULL) {f = "(nil)";}

    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node->next = NULL;
    node->prev = NULL;
    node->val = val;
    node->name = f;

    return node;
}
struct LinkedList* LinkedListInitializer(int val, char *f)
{
    if(f == NULL) {f = "(nil)";}

    struct Node* node = NodeInitializer(val, f);
    struct LinkedList* list = (struct LinkedList*) malloc(sizeof(struct LinkedList));

    list->head = node;
    list->end = node;
    list->size = 1;
    return list;
}

void AddFront(struct LinkedList *list, struct Node* node)
{
    if(list == NULL || node == NULL) {return ;}

    node->next = list->head;
    list->head->prev = node;
    list->head = node;
    list->size = list->size + 1;
}

void AddBack(struct LinkedList *list, struct Node* node) {
    if(list == NULL || node == NULL) {return ;}

    node->prev = list->end;
    list->end->next = node;
    list->end = node;
    list->size = list->size + 1;
}

struct Node* FindNode(struct LinkedList *list, int n)
{
    if (list == NULL) {return NULL;}

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
    if (n == NULL || *li == NULL) {return;}

    struct LinkedList *li_tmp = *li;
    li_tmp->size = li_tmp->size - 1;

    if(li_tmp->size == 0) {
        free(n);
        free(*li);
        *li = NULL;
        return;
    }

    if(li_tmp->head->val == n->val) {
        li_tmp->head = n->next;
        n->next->prev = NULL;
        goto DeleteNode_end;
    }
    
    else if(li_tmp->end->val == n->val) {
        li_tmp->end = n->prev;
        n->prev->next = NULL;
        goto DeleteNode_end;
    }

    n->next->prev = n->prev;
    n->prev->next = n->next;
    
    DeleteNode_end:
    free(n);
}

void PrintLinkedList(struct LinkedList *li){
    if(li == NULL) {
        printf("Empty Process List\n");
        return;
    }

    struct Node* n = li->head;    
    while (n != NULL) {
        PrintNode(n);
        n = n->next;
    }
}

void PrintNode(struct Node *node) {
    if (node == NULL) {printf("emptry node error\n"); return;}

    int pid = node->val;
    ssize_t len, alloclen = 128;

    char path[MAX_INT_STR_LENGTH], *cwd;
    sprintf(path, "/proc/%d/cwd", pid);

    if (fopen(path, "r") == NULL) {
        printf("%d: %s\n", pid, node->name);
        return;
    }

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