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
#define DESCRIPTOR_SIZE 40
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#define align8(x) (((((x)-1) >> 3) << 3) + 8)

typedef struct block_descriptor* descriptor_ptr;
//descriptor_ptr <-> struct block_descriptor*

struct block_descriptor {
    // free=1 Available, free=0 Unavailable
    long free; //8 bytes
    // size of the block
    size_t size; //8 bytes
    // The next allocated block
    descriptor_ptr next; //8 bytes
    // The previous allocated block
    descriptor_ptr prev; //8 bytes
    // ptr is used to align the memory to 4. This pointer
    // will be used to access the actual storage
    void *ptr; // bytes
    // The first byte of the actual memory
    char block[1]; //1 bytes
};

// The start of the list, initially empty
void *head = NULL;

void split_block(descriptor_ptr block, size_t size) {
    descriptor_ptr new_block;
    new_block = (descriptor_ptr)(block->block+size); // The new block is going to be the free space remaining
    new_block->size = block->size - size - DESCRIPTOR_SIZE; // The size is going to be the original size - the size allocated - the new DESCRIPTOR_SIZE required for the new block
    new_block->next = block->next; // The next pointer of the free block is going to point to the same next block of the initial block
    new_block->free = 1;
    new_block->ptr = new_block->block; // Ptr is an alias for the starting point of the actual storage space
    new_block->prev = block; // The previous block of the free one is the one we are currently allocating
    block->next = new_block; // Viceversa
    block->size = size;
    if (new_block->next) // If the next block exists, then we set the new free one to be the prev of the next
    {
        new_block->next->prev = new_block;
    }
}

//This function returns the best block that fits the request
// We use pointer to pointer to be able to change the block start points to from the caller function
descriptor_ptr find_suitable_block(descriptor_ptr *start, size_t size) // start is going to be set, after the function, at the very end of the list
{
    //The start of the list and of the search
    descriptor_ptr b = head;
    size_t best_size = INT_MAX;
    //If there are blocks left in the list
    while(b->next)
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
    //Extra if to check for the last
    if(b->free && b->size >= size)
        if( b->size < best_size) {
            *start = b;
            return b;
        }
    // If a suited block has been found
    if (best_size != INT_MAX){
        if(best_size > size + DESCRIPTOR_SIZE)
            split_block(*start, size);
        return *start;
    }
    else {
        *start=b;
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
    block->prev = tail;
    block->ptr = block->block;
    // If the list is not empty, set the 
    if (tail)
    {
        tail->next = block;
    }
    return (block);
}


void *fmalloc(size_t size) {
    pthread_mutex_lock(&lock);

    descriptor_ptr block, tail;
    // We only allocate memory in multiple of 8 bytes for efficieny
    size_t s;
    s = align8(size);

    // If the list is empty, allocate the first block
    if(head==NULL) {
        block=extend_heap(NULL, s);
        if (!block)
        {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        head = block;
    } 
    else {
        // If the list is not empty
        tail = head;
        block = find_suitable_block(&tail, s); // After this call, tail is modified due to the pointer to pointer arguement, and can be used in extend_heap afterwards
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
                pthread_mutex_unlock(&lock);
                return NULL;
            }
        }
    }
    
    descriptor_ptr b = head;
    pthread_mutex_unlock(&lock);
    return block->block;
}