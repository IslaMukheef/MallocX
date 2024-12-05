#include <stdio.h>
#include "malloc.h"

int main() {
    printf("Testing malloc:\n");
    void *ptr1 = malloc(32);
    printf("ptr1 = %p\n", ptr1);

    printf("Testing calloc:\n");
    void *ptr2 = calloc(4, 8);
    printf("ptr2 = %p\n", ptr2);

    printf("Testing realloc:\n");
    void *ptr3 = realloc(ptr2, 64);
    printf("ptr3 = %p\n", ptr3);

    printf("Freeing memory:\n");
    free(ptr1);
    free(ptr3);

    return 0;
}

