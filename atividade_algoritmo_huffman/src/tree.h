#ifndef __H_TREE__

#define __H_TREE__

#include <stdio.h>
#include <stdlib.h>
#include "./defs.h"

struct TNode {

    short label;
    BOOL edge;
    uint_t fr_sum;

    struct TNode *father,
                 *left, *right;

};

typedef struct TNode TNode;

struct THuffman {
    TNode *root;

    void (*str)(struct THuffman *th);
    void (*destroy)(struct THuffman **th);
};

typedef struct THuffman THuffman;

THuffman *new_thuffman();
TNode    *new_node(uint_t fr_sum);
TNode    *get_leaf(THuffman *th, BYTE symbol);
TNode    *get_node_bin(TNode *node, uint_t bit);
BOOL      is_leaf(TNode *node);

#endif
