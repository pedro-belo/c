#ifndef __PILHA_H_

#define __PILHA_H_

#include <stdlib.h>
#include <stdio.h>

typedef int boolean;

struct Item {
    int value;
    struct Item *next;  
};


struct Stack {

    struct Item *items;

};

struct Stack *create(struct Stack *s);
boolean is_empty(struct Stack *s);
int top(struct Stack *s);
void push(struct Stack *s, int value);
int pop(struct Stack *s);
void destroy(struct Stack *s);

#endif