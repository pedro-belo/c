#ifndef __LINKED_LIST_H_
#define __LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef int boolean;

struct Item{

    int value;

    struct Item *next;

};


struct List{

    struct Item *items;

};

struct List *create(struct List *list);
boolean is_empty(struct List *list);
void append(struct List *list, int value);
unsigned int count(struct List *list);
int get(struct List *list, unsigned int pos);
void print_ls(struct List *list);
int update(struct List *list, unsigned int pos, int value);
void destroy(struct List *list);
void rem(struct List *list, unsigned int pos);

#endif