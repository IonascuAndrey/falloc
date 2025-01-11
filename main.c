#include <stdio.h>
#include "fmalloc.h"
#include "ffree.h"
#include "frealloc.h"

int main() {
    // Allocate memory for 128 bytes
    void *ptr = fmalloc(128);
    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1; // Exit with error if allocation fails
    }
    printf("Allocated memory at %p\n", ptr);

    // Reallocate memory to 256 bytes
    ptr = frealloc(ptr, 256);
    if (ptr == NULL) {
        printf("Memory reallocation failed\n");
        return 1; // Exit with error if reallocation fails
    }
    printf("Reallocated memory at %p\n", ptr);

    // Free the allocated memory
    ffree(ptr);
    printf("Freed memory at %p\n", ptr);

    return 0; // Return success
}
