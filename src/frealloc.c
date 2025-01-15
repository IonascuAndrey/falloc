#include "fmalloc.h"
#include "frealloc.h"
#include "ffree.h"
#include <string.h>
#include <stdio.h>

void *frealloc(void *ptr, size_t size) {
    if (!ptr) {
        // Equivalent to malloc if ptr is NULL
        return fmalloc(size);
    }

    void *new_ptr = fmalloc(size);
    if (new_ptr) {
        // Copy the old data to the new location (size-limited)
        memcpy(new_ptr, ptr, size);
    }
    ffree(ptr);
    return new_ptr;
}
