#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void print_blocks(void);

#endif