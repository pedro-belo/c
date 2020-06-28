#include "./list.h"

static struct Item *create_item(int value) {

    struct Item *i = NULL;

    if((i=(struct Item *)malloc(sizeof(struct Item))) == NULL){
        return NULL;
    }

    i->value = value;
    i->next = NULL;

    return i;
}

static void insert_start(struct List *list, struct Item *i) {
    list->items = i;
}

static void insert_end(struct List *list, struct Item *i) {

    struct Item *tmp = list->items;

    while(tmp->next)
        tmp = tmp->next;

    tmp->next = i;
}

static struct Item *get_item(struct List *list, unsigned int pos) {

    struct Item *item = NULL;
    unsigned int count = 1;

    item = list->items;

    while(item && count != pos){
        item = item->next;
        count += 1;
    }

    return item;
}

struct List *create(struct List *list) {

    if(list == NULL) {
        list = (struct List *)malloc(sizeof(struct List));
        list->items = NULL;
        return list;
    }

    list->items = NULL;
    return NULL;
}

void destroy(struct List *list) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    struct Item *tmp;

    while(list->items) {
        tmp = list->items;
        list->items = tmp->next;
        free(tmp);
    }

}

boolean is_empty(struct List *list) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    return list->items == NULL;
}

unsigned int count(struct List *list) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    struct Item *tmp;
    unsigned int total_items = 0;

    tmp = list->items;
 
    while(tmp){
        tmp = tmp->next;
        total_items += 1;
    }

    return total_items;
}

int get(struct List *list, unsigned int pos) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    if(is_empty(list)) { 
        fprintf(stderr, "list is empty");
        exit(EXIT_FAILURE);        
    }

    struct Item *i = get_item(list, pos);

    if(i == NULL) { 
        fprintf(stderr, "null pointer");
        destroy(list);
        exit(EXIT_FAILURE);        
    }

    return i->value;
}

int update(struct List *list, unsigned int pos, int value) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    if(is_empty(list)) { 
        fprintf(stderr, "list is empty");
        exit(EXIT_FAILURE);        
    }

    struct Item *i = get_item(list, pos);
    int old;

    if(i == NULL) { 
        fprintf(stderr, "null pointer");
        destroy(list);
        exit(EXIT_FAILURE);        
    }

    old = i->value;
    i->value = value;

    return old;
}

void print_ls(struct List *list) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    unsigned int len = count(list);

    puts("[");
    for (size_t i = 1; i <= len; i++)
    {
        printf("\t%u -> %d,\n", i, get(list, i));
    }
    puts("]");

}

void rem(struct List *list, unsigned int pos) {

    if(list == NULL){
        fprintf(stderr, "list is null");
        exit(EXIT_FAILURE);
    }

    if(is_empty(list)){
        fprintf(stderr, "list is empty");
        exit(EXIT_FAILURE);
    }

    struct Item *i = NULL, *j = NULL;

    if( pos == 1 ){
        i = list->items;
        list->items = list->items->next;
        free(i);
        return;
    }

    i = get_item(list, pos - 1);
    if(i->next == NULL){
        fprintf(stderr, "invalid position!");
        destroy(list);
        exit(EXIT_FAILURE);        
    }

    j = i->next->next;

    free(i->next);
    i->next = j;

}

void append(struct List *list, int value) {

    if(list == NULL) { 
        fprintf(stderr, "null pointer");
        exit(EXIT_FAILURE);        
    }

    struct Item *item = NULL;

    if((item=create_item(value)) == NULL) {
        fprintf(stderr, "insufficient memory");
        destroy(list);
        exit(EXIT_FAILURE);
    }

    is_empty(list) ? insert_start(list, item) : insert_end(list, item);    
}