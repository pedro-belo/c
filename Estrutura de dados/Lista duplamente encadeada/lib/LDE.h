#ifndef __H_LDE__

#define __H_LDE__

#include <stdio.h>
#include <stdlib.h>

#define INICIO 1
#define FIM 2

struct Item
{
    int value;
    struct Item *previous, *next;
};

struct LDE
{
    unsigned int length;
    struct Item *first, *last;
};

struct LDE *lde_init();
int is_empty(struct LDE *lde);
void print_lde(struct LDE *lde);
struct LDE *inserir(struct LDE *lde, int value, int pos);
struct LDE *remove_item(struct LDE *lde, int value);

#endif