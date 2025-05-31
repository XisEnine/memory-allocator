#include <stdio.h>
#include "malloc.h"

int main()
{
    int *a = (int *)my_malloc(sizeof(int));
    *a = 42;
    printf("%d", *a);
    my_free(a);
    print_blocks();
    return 0;
}