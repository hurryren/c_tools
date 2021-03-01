#include<stdlib.h>
#include <stdio.h>
#include<string.h>

int main(){
    int a;
    void *p;

    printf("address of a:%p\n",(void *)&a);
    p = malloc(98);
    if(p == NULL){
        fprintf(stderr,"can't malloc");
        return (EXIT_FAILURE);
    }
    printf("Allocated space in the heap: %p\n",p);
    
    return (EXIT_SUCCESS);
}
