#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int ac, char **av, char **env)
{
    int a;
    void *p;
    int i;

    printf("address of a:%p\n", (void *)&a);
    p = malloc(98);
    if (p == NULL)
    {
        fprintf(stderr, "can't malloc");
        return (EXIT_FAILURE);
    }
    printf("Allocated space in the heap: %p\n", p);
    printf("Address of function main: %p\n", (void *)main);
    printf("first byte of main function: \n\t");
    for (i = 0; i < 15; i++)
    {
        printf("%02x ",((unsigned char *)main)[i]);
    }
    printf("\n");

    printf("address of the array of arguments: %p\n",(void *)av);
    printf("address of the  arguments: \t\n");
    for(i = 0;i<ac;i++){
        printf("[%s]:%p\n",av[i],av[i]);
    }
    printf("\n");
    printf("address of the array of environment variables : %p\n",(void *)env);
    printf("address of the first  environment variables : %p\n",(void *)(env[0]));
    return (EXIT_SUCCESS);
}
