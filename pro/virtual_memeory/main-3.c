#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int a;
    void *p;
    unsigned int i;

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

    return (EXIT_SUCCESS);
}
