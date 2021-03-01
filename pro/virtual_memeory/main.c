#include <stdlib.h>
#include<stdio.h>
#include<string.h>

int main(void){
    char *s;

    s = strdup("helberton");
    if (s == NULL){
        fprintf(stderr,"can't allocate mem with malloc");
        return (EXIT_FAILURE);
    }
    printf("%p\n",(void *)s);
    return (EXIT_SUCCESS);

}


