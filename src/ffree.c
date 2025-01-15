#include "ffree.h"
#include "fmalloc.c"
#include <stdio.h>

// returns the adress of the descriptor
descriptor_ptr get_block_address(void *p)
{
    char *descriptor_start = p; // Convert to char for pointer arithemtic
    descriptor_start = descriptor_start - DESCRIPTOR_SIZE;
    p = descriptor_start;
    return (p);
}
// *p is the address if the block itself
int is_addr_valid(void *p)
{
    char *ptr = p;
    descriptor_ptr current=head;
    while (current)
    {
        // Check if the pointer is within the current block's range
        // and ensure the pointer is inside the mapped memory (page-aligned)
        if (ptr >= current->block && ptr < (char *)current->block + current->size) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

descriptor_ptr merge_blocks(descriptor_ptr block)
{
    printf("Final anterior: %p, Inceput urmator %p \n" ,block->block+block->size+1, block->next);
    if (block->next && block->next->free && block) // If the next block exists and it is free
    {
        block->size += DESCRIPTOR_SIZE + block->next->size; // merge
        block->next = block->next->next;
    }
    if (block->next)
    {
        block->next->prev = block;
    }
    block->free=1;
    return block;
}

void cascade_unmap(descriptor_ptr tail) {
    if (tail == NULL) {
        printf("Reached the start of the list or NULL block.\n");
        return;
    }

    printf("Inspecting block at %p, size: %zu, free: %d\n", tail, tail->size, tail->free);

    if (tail->free) {
        printf("Unmapping block at %p\n", tail);

        // Save the previous block pointer before unmapping
        descriptor_ptr prev = tail->prev;

        munmap(tail, tail->size + DESCRIPTOR_SIZE);

        // Recursively unmap the previous block
        if (prev) {
            printf("Moving to previous block at %p\n", prev);
            cascade_unmap(prev);
        } else {
            printf("No previous block to unmap.\n");
            head=NULL; // To remove the dangling pointer
        }
    } 
    else {
        printf("Block at %p is not free, stopping cascade.\n", tail);
        tail -> next = NULL;
    }
}
// This is a no-op in this basic example
void ffree(void *ptr)
{
    pthread_mutex_lock(&lock);
    
    if (is_addr_valid(ptr))
    {
        descriptor_ptr block = get_block_address(ptr);
        block->free = 1;

        if (block->prev && block->prev->free && block->prev->block+block->prev->size==block) // the previous block exists, it is free, and mergable
        {
            block = merge_blocks(block->prev);
        }
        if (block->next && block->block+block->size==block->next)
        {
            block = merge_blocks(block);
        } 
        if(!block->next){
            // Unmapping all the last free consecutive blocks from the list
            cascade_unmap(block);
        }
    }
    pthread_mutex_unlock(&lock);
}