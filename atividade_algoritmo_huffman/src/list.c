#include "./list.h"

static void str(List *list);
static void _add(List *list, TNode *node);

List *new_list() {

    List *list = (List *)malloc(sizeof(List));

    if(!list)
        return NULL;

    list->items = NULL;

    list->str = &str;

    return list;
}

int is_empty(List *list){
    return !list->items;
}

void add(List *list, TNode *node) {
    _add(list, node);
    sort(list);
}

void sort(List *list) {

    ItemList *current = list->items;
    TNode *bak = NULL;

    if(!current)
        return;

    while( current->next ) {

        if(current->node->fr_sum > current->next->node->fr_sum) {
            bak = current->node;
            current->node = current->next->node;
            current->next->node = bak;
        }

        current = current->next;
    }

}

TNode *rem(List *list) {

    if(!list->items)
        return NULL;

    ItemList *item = list->items;
    TNode *node = item->node;

    list->items = item->next;
    free(item);

    sort(list);

    return node;
}

THuffman *build(List *list) {

    THuffman *th = NULL;
    th = new_thuffman();

    TNode *n1, *n2, *new;
    n1 = n2 = new = NULL;

    while(list->items->next){

        n1 = rem(list);
        n2 = rem(list);      

        new = new_node(n1->fr_sum + n2->fr_sum);
        n1->father = n2->father = new;

        if(n1->fr_sum > n2->fr_sum){
            new->edge = 0;

            new->left = n1;
            new->right = n2;

            n1->edge = 0;
            n2->edge = 1;

        }else{

            new->edge = 1;
            new->left = n2;
            new->right = n1;

            n1->edge = 1;
            n2->edge = 0;
        }

        add(list, new);
    }

    th->root = list->items->node;
    list->items = NULL;

    return th;
}

static void str(List *list){

    ItemList *item = list->items;

    while(item){
        fprintf(stdout, " [%#x:%i] ", item->node->label, item->node->fr_sum);
        item = item->next;
    }
    fprintf(stdout, "\n");
}

static void _add(List *list, TNode *node) {

    ItemList *item = NULL;

    item = (ItemList *)malloc(sizeof(ItemList));

    if(!item){
        fprintf(stderr, "Alocação de memória falhou.\n");
        exit(R_ERROR);
    }

    item->node = node;
    item->next = NULL;

    if(!list->items) {
        item->node = node;
        list->items = item;
        return;
    }

    item->next = list->items;
    list->items = item;
}