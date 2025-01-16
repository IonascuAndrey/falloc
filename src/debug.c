#include "debug.h"
#include "ffree.c"
#include <stdio.h>

void print_memory_list()
{
    descriptor_ptr start = head;
    printf("---------------------------\n");
    while(start) {
        
        printf("Block Descriptor %p:\n", start);
        printf("  free: %ld\n", start->free);
        printf("  size: %zu\n", start->size);
        printf("  next: %p\n", (void *)start->next);
        printf("  prev: %p\n", (void *)start->prev);
        printf("  ptr: %p\n", start->ptr);
        printf("  lock: %p\n", start->block);
        start=start->next;
        
    }
    printf("---------------------------\n\n\n");
}
