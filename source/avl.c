#include <stdlib.h>
#include<string.h>

#include "avl.h"

static void destroy_right(BisTree *tree,BiTreeNode *node);

static void rotate_left(BiTreeNode **node){
    BiTreeNode *left,*grandchild;
    left = bitree_left(*left);
    if((AvlNode *)(bitree_data(left))->factor == AVL_LEFT_HEAVY){

    }
}