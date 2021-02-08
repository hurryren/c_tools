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
}