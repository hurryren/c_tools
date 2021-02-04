#ifndef LIST_H
#define LIST_H

#include<stdlib.h>

typedef struct ListElmt_{
        void                    *data;
        struct ListElmt_        *next;

}ListElmt;

typedef struct List_{
        int size;
        int (*match)(const void *key1, const void *key2);
        void (*destroy)(void *data);
        ListElmt *head;
        ListElmt *tail;
}List;






#endif
