#include "fmalloc.h"
#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#define INT_MAX 2147483647
/*
 * This implementation of Alloc library uses best-fit technique
 * The data is stored in a double linked list of block descriptors.
 * The size of a descriptor is 20 bytes
 */
#define DESCRIPTOR_SIZE 20
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define align4(x) (((((x)-1) >> 2) << 2) + 4)

typedef struct block_descriptor *descriptor_ptr;

struct block_descriptor {
    // free=1 Available, free=0 Unavailable
    int free;
    // size of the block
    size_t size;
    // The next allocated block
    descriptor_ptr next;
    // The previous allocated block
    descriptor_ptr prev;
    // ptr is used to align the memory to 4. This pointer
    // will be used to access the actual storage
    void *ptr;
    // The first byte of the actual memory
    char block[1];
};

// The start of the list, initially empty
void *head = NULL;

//This function returns the best block that fits the request
descriptor_ptr find_suitable_block(descriptor_ptr *start, size_t size)
{
    //The start of the list and of the search
    descriptor_ptr b = head;
    size_t best_size = INT_MAX;
    //If there are blocks left in the list
    while(b)
    {
        //if the block is free and can fit the request
        if(b->free && b->size >= size)
            // and it is a better fit than the last
            if( b->size < best_size) {
                *start = b;
                best_size = b->size;
            }
        b = b->next;
    }
    // If a suited block has been found
    if (best_size != INT_MAX)
        return *start;
    else {
        return NULL;
    }
}

descriptor_ptr extend_heap(descriptor_ptr tail, size_t size)
{
    size_t total_size = size + DESCRIPTOR_SIZE;
    descriptor_ptr block;
    block = mmap(0, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }
    block->size = size;
    block->free = 0;
    block->next = NULL;
    block->prev = NULL;
    block->ptr = block->block;
    // If the list is not empty, set the 
    if (tail)
    {
        tail->next = block;
    }
    return (block);
}


void *fmalloc(size_t size) {
    printf("Merge MALLOC");
    pthread_mutex_lock(&lock);

    descriptor_ptr block, tail;
    // We only allocate memory in multiple of 4 chunks for efficieny
    size_t s;
    s = align4(size);

    // If the list is empty, allocate the first block
    if(head==NULL) {
        block=extend_heap(NULL, s);
        if (!block)
        {
            return NULL;
        }
        head = block;
    } 
    else {
        // If the list is not empty
        tail = head;
        block = find_suitable_block(&tail, s);
        if (block)
        {
            //If the block was found
            // if (block->size - s >= (META_BLOCK_SIZE + 4))
            // {
            //     split_space(block, s);
            // }
            block->free = 0;
        }
        else
        {
            // If no suitable block was found, tail is guaranteed to be the last block
            block = extend_heap(tail, s);
            if (!block)
            {
                return NULL;
            }
        }
    }
    pthread_mutex_unlock(&lock);
    return block->block;
}
