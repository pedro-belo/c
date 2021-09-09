#include "./tree.h"

// Prototypes
static void _str(TNode *node);
static void str(THuffman *th);
static void _destroy(TNode **node);
static void destroy(THuffman **th);
static TNode *_get_leaf(TNode *node, BYTE symbol);


THuffman *new_thuffman(){

    THuffman *th = NULL;
    th = (THuffman *)malloc(sizeof(THuffman));

    if(!th)
        return NULL;

    th->root = NULL;
    th->str = &str;
    th->destroy = &destroy;

    return th;
}

TNode *new_node(uint32_t fr_sum) {

    TNode *node = NULL;
    node = (TNode *)malloc(sizeof(TNode));

    if(!node)
        return NULL;

    node->edge = -1;
    node->label = -1;
    node->left = node->right = NULL;
    node->fr_sum = fr_sum;

    return node;
}

TNode *get_node_bin(TNode *node, uint32_t bit) {
    return bit > 0 ? node->right : node->left;
}

BOOL is_leaf(TNode *node) {
    return !node->left && !node->right;
}

TNode *get_leaf(THuffman *th, BYTE symbol){
    return _get_leaf(th->root, symbol);
}


// Static functions
static void str(THuffman *th){
    _str(th->root);
}

static void _str(TNode *node) {

    if(!node){
        return;
    }

    if(!node->left && !node->right)    
        fprintf(stdout, "Node(fr=%d, edge=%d, label=%#x) \n", node->fr_sum, node->edge, node->label);
    else
        fprintf(stdout, "Node(fr=%d, edge=%d) \n", node->fr_sum, node->edge);
    
    fprintf(stdout, "\n\tESQUERDA\n\n");
    _str(node->left);

    fprintf(stdout, "\n\tDIREITA\n\n");
    _str(node->right);

    fprintf(stdout, "\n\tVOLTANDO\n\n");
}

static TNode *_get_leaf(TNode *node, BYTE symbol) {

    if(!node){
        return NULL;
    }

    TNode *ptr = NULL;

    if(node->label == symbol)
        return node;

    ptr = _get_leaf(node->left, symbol);
    if(ptr) return ptr;

    ptr = _get_leaf(node->right, symbol);
    if(ptr) return ptr;

    return NULL;
}

static void destroy(THuffman **th) {
    
    _destroy(&(*th)->root);

    free(*th);
    *th = NULL;
}

static void _destroy(TNode **node){

    if(!node || !(*node))
        return;

    _destroy(&(*node)->left);
    _destroy(&(*node)->right);

    (*node)->father = NULL;
    (*node)->left = NULL;
    (*node)->right = NULL;

    free(*node);
}
