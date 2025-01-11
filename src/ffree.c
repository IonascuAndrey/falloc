#include "ffree.h"
#include "fmalloc.c"
#include <stdio.h>

// returns the adress of the descriptor
descriptor_ptr get_descriptor_address(void *p)
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
    if (current)
    {
        // Check if the pointer is within the current block's range
        // and ensure the pointer is inside the mapped memory (page-aligned)
        if (ptr >= current->block && ptr < (char *)current + current->size) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}
// This is a no-op in this basic example
void ffree(void *ptr)
{
    printf("Merge FREE");
    if (is_addr_valid(ptr))
    {
        descriptor_ptr descriptor = get_descriptor_address(ptr);
        descriptor->free = 1;
        
    }
}