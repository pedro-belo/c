#include "LDE.h"

struct LDE *lde_init()
{

    struct LDE *lde = NULL;

    lde = (struct LDE *)calloc(sizeof(struct LDE *), 1);

    if (!lde)
    {
        fprintf(stderr, "%d: %s -> calloc()\n", __LINE__, __FILE__);
        exit(-1);
    }

    return lde;
}

int is_empty(struct LDE *lde)
{
    return lde->length == 0;
}

static struct Item *new_item(int value)
{

    struct Item *item = NULL;

    item = (struct Item *)malloc(sizeof(struct Item *));
    if (!item)
    {
        fprintf(stderr, "%d: %s -> calloc()\n", __LINE__, __FILE__);
        exit(-1);
    }

    item->previous = item->next = NULL;
    item->value = value;

    return item;
}

static struct LDE *inserir_inicio(struct LDE *lde, int value)
{

    struct Item *item = new_item(value);

    if (is_empty(lde))
    {
        lde->first = lde->last = item;
        lde->length++;
        return lde;
    }

    item->next = lde->first;
    lde->first->previous = item;
    lde->first = item;
    lde->length++;
    return lde;
}

static struct LDE *inserir_final(struct LDE *lde, int value)
{

    struct Item *item = new_item(value);

    if (is_empty(lde))
    {
        lde->first = lde->last = item;
        lde->length++;
        return lde;
    }

    item->previous = lde->last;
    lde->last->next = item;
    lde->last = item;
    lde->length++;
    return lde;
}

struct LDE *inserir(struct LDE *lde, int value, int pos)
{

    switch (pos)
    {
    case INICIO:
        return inserir_inicio(lde, value);

    case FIM:
        return inserir_final(lde, value);
    }

    return NULL;
}

void print_lde(struct LDE *lde)
{

    if (!is_empty(lde))
    {

        int count = 1;

        struct Item *tmp = lde->first;

        while (tmp)
        {
            fprintf(stdout, "LISTA[%.2d] = %d\n", count++, tmp->value);
            tmp = tmp->next;
        }
    }
}

static void remove_inicial(struct LDE *lde){
    lde->first = lde->last = NULL;
}

static void remove_inicio(struct LDE *lde){
    lde->first = lde->first->next;
    lde->first->previous = NULL;
}

static void remove_final(struct LDE *lde){
    lde->last = lde->last->previous;
    lde->last->next = NULL;
}

static void remove_meio(struct Item *item){
    item->next->previous = item->previous;
    item->previous->next = item->next;
}

struct LDE *remove_item(struct LDE *lde, int value){

    if(is_empty(lde)){
        fprintf(stdout, "Vazio!");
        return lde;
    }

    struct Item *item = lde->first;

    while ( item && item->value != value) item = item->next;
    
    if(!item){
        fprintf(stdout, "Item nao encontrado.");
        return lde;
    }

    if( !item->previous && !item->next){
        remove_inicial(lde);
    }else
    if( item->previous && !item->next ){
        remove_final(lde);
    }else
    if( !item->previous && item->next ){
        remove_inicio(lde);
    }else{
        remove_meio(item);
    }

    lde->length--;
    free(item);

    return lde;
}