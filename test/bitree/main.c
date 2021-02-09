#include<stdio.h>
#include<stdlib.h>

#include"bitree.h"
#include"traverse.h"

static void print_list(const List *list){
    ListElmt *element;

    int *data, i;

    fprintf(stdout, "-> list size is %d\n",list_size(list));

    i = 0;
    element = list_head(list);

    while(1){
        data = list_data(element);
        fprintf(stdout,"--> list[%03d]=%03d\n",i,*data);

        i++;

        if(list_is_tail(list, element)){
            break;
        }else{
            element = list_next(element);
        }
    }
}

typedef enum {BiTreePreOrder,BiTreeInOrder,BiTreePostOrder}BiTreeOrderType;
static void print_tree(const BiTreeNode *node, BiTreeOrderType type){
    List list;
    list_init(&list,free);

    switch(type){
        case BiTreePreOrder:
            // 前序遍历
            bitree_preorder(node,&list);
            break;
        case BiTreeInOrder:
            // 前序遍历
            bitree_inorder(node,&list);
            break;
        case BiTreePostOrder:
            // 前序遍历
            bitree_postorder(node,&list);
            break;
        default:
            return;
    }
    return print_list(&list);
}

static void print_preorder(const BiTreeNode *node){
    return print_tree(node, BiTreePreOrder);
}


static void print_inorder(const BiTreeNode *node){
    return print_tree(node, BiTreeInOrder);
}


static void print_postorder(const BiTreeNode *node){
    return print_tree(node, BiTreePostOrder);
}

static int insert_int(BiTree *tree, int i){
    BiTreeNode *node,*prev = NULL;

    int direction,*data;

    node = tree->root;
    direction = 0;
    while(!bitree_is_eob(node)){
        prev = node;
        if(i == *(int *)bitree_data(node)) return -1;

        if(i < *(int *)bitree_data(node)){
            node = bitree_left(node);
            direction = 1;
        }else{
            node = bitree_right(node);
            direction = 2;
        }
    }
    if((data = (int *)malloc(sizeof(int))) == NULL) return -1;

    *data = i;

    if(direction == 0) return bitree_ins_left(tree, NULL,data);
    if(direction == 1) return bitree_ins_left(tree, prev,data);
    if(direction == 2) return bitree_ins_right(tree, prev,data);

    return -1;
}

static  BiTreeNode *search_int(BiTree *tree, int i){
    BiTreeNode *node;

    node = bitree_root(tree);
    while(!bitree_is_eob(node)){
        if(i == *(int *)bitree_data(node)) return node;

        if(i < *(int *)bitree_data(node)){
            node = bitree_left(node);
        }else{
            node = bitree_right(node);
        }
    }

    return NULL;
}

int main(int argc , char **argv){
    BiTree tree;
    BiTreeNode *node;

    int i;




    bitree_init(&tree, free);
    fprintf(stdout,"Inserting some nodes\n");
    if(insert_int(&tree,20) != 0) return 1;
    if(insert_int(&tree,10) != 0) return 1;
    if(insert_int(&tree,30) != 0) return 1;
    if(insert_int(&tree,15) != 0) return 1;
    if(insert_int(&tree,25) != 0) return 1;
    if(insert_int(&tree,70) != 0) return 1;
    if(insert_int(&tree,80) != 0) return 1;
    if(insert_int(&tree,23) != 0) return 1;
    if(insert_int(&tree,26) != 0) return 1;
    if(insert_int(&tree,5) != 0) return 1;

    fprintf(stdout,"tree size is %d\n",bitree_size(&tree));
    fprintf(stdout,"(preoeder traversal)\n");
    print_preorder(bitree_root(&tree));

    i = 30;

    if((node = search_int(&tree, i)) == NULL){
        fprintf(stdout,"could not find %03d\n",i);
    }else{
        fprintf(stdout,"found %03d ... removing the left tree below it\n",i);
        bitree_rem_left(&tree,node);
        fprintf(stdout,"tree size is %d\n",bitree_size(&tree));
        fprintf(stdout,"(pre0rder traversal)\n");
        print_preorder(bitree_root(&tree));
    }


}