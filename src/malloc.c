#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

struct block_meta
{
    size_t size;
    struct block_meta *next;
    int free;
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

// Find a free block of memory
struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
    struct block_meta *current = global_base;
    while (current && !(current->free && current->size >= size))
    {
        *last = current;
        current = current->next;
    }
    return current;
}

// Request new space using sbrk
struct block_meta *request_space(struct block_meta *last, size_t size)
{
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    if (request == (void *)-1)
    {
        return NULL;
    }
    assert(block == request);
    if (last)
    {
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    return block;
}

// Custom malloc
void *my_malloc(size_t size)
{
    struct block_meta *block;
    if (size <= 0)
    {
        return NULL;
    }
    if (!global_base)
    {
        block = request_space(NULL, size);
        if (!block)
        {
            return NULL;
        }
        global_base = block;
    }
    else
    {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block)
        {
            block = request_space(last, size);
            if (!block)
            {
                return NULL;
            }
        }
        else
        {
            block->free = 0;
        }
    }
    return (block + 1);
}

// Get metadata from user pointer
struct block_meta *get_block_ptr(void *ptr)
{
    return (struct block_meta *)ptr - 1;
}

// Custom free
void my_free(void *ptr)
{
    if (!ptr)
    {
        return;
    }
    struct block_meta *block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    block_ptr->free = 1;
}

// Debug print to view memory state
void print_blocks()
{
    struct block_meta *current = global_base;
    printf("\n[Memory Blocks]\n");
    while (current)
    {
        printf("Block %p | size: %zu | free: %d | next: %p\n",
               (void *)current, current->size, current->free, (void *)current->next);
        current = current->next;
    }
    printf("---------------------------\n");
}
