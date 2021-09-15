#ifndef __H_LIST__

#define __H_LIST__

#include <stdio.h>
#include <stdlib.h>
#include "./tree.h"

struct ItemList {
    TNode *node;
    struct ItemList *next;
};

typedef struct ItemList ItemList;

struct List {
    ItemList *items;

    void (*str)(struct List *list);
};

typedef struct List List;

List *new_list();
void sort(List *list);
TNode *rem(List *list);
int is_empty(List *list);
THuffman *build(List *list);
void add(List *list, TNode *node);

#endif
