#ifndef TRAVERSE_H
#define TRAVERSE_H
#include"bitree.h"
#include"list.h"
// 前序遍历
int bitree_preorder(const BiTreeNode *node, List *list);

// 中序遍历
int bitree_inorder(const BiTreeNode *node, List *list);

// 后序遍历
int bitree_postorder(const BiTreeNode *node, List *list);


#endif