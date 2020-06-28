#include "./stack.h"

static struct Item *create_item(int value, struct Item *next) {

    struct Item *item = NULL;

    item = (struct Item *)malloc(sizeof(struct Item));

    if(item == NULL)
        return NULL;

    item->value = value;
    item->next = next;

    return item;
}

boolean is_empty(struct Stack *s) {

    if(s == NULL){
        fprintf(stderr, "Stack is null!");
        exit(EXIT_FAILURE);
    }

    return s->items == NULL;
}

void destroy(struct Stack *s) {

    if(s == NULL){
        fprintf(stderr, "Stack is null!");
        exit(EXIT_FAILURE);
    }

    while (!is_empty(s))
        pop(s);
}

int pop(struct Stack *s) {

    if(s == NULL){
        fprintf(stderr, "Stack is null !");
        exit(EXIT_FAILURE);
    }

    if(is_empty(s)){
        fprintf(stderr, "Stack is empty !");
        exit(EXIT_FAILURE);
    }

    struct Item *tmp = NULL;
    int ret;

    tmp = s->items;
    s->items = tmp->next;
    
    ret = tmp->value;

    tmp->next = NULL;
    free(tmp);

    return ret;
}

int top(struct Stack *s) {

    int value;

    value = pop(s);
    push(s, value);

    return value;
}

void push(struct Stack *s, int value) {

    if(s == NULL){
        fprintf(stderr, "pop -> Stack is null !");
        exit(EXIT_FAILURE);
    }

    struct Item *i = NULL;
    i = is_empty(s) ? create_item(value, NULL) : create_item(value, s->items);

    if(i == NULL){
        destroy(s);
        exit(EXIT_FAILURE);
    }

    s->items = i;
}

struct Stack *create(struct Stack *s) {

    if(s == NULL) {
       s = (struct Stack *)malloc(sizeof(struct Stack));
       s->items = NULL;
       return s;
    }

    s->items = NULL;
    return NULL;
}