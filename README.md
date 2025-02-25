# Falloc - Custom Memory Allocator

Falloc is a lightweight and efficient memory allocation library written in C. It provides an alternative to standard `malloc`, `free`, `realloc`, and `calloc`, offering improved performance and memory management features.

## Features

- Efficient memory allocation with minimal fragmentation
- Thread-safe allocation (if implemented)
- Simple API similar to `malloc`/`free`/`realloc`

## Installation

### Prerequisites
- C compiler (GCC/Clang/MSVC)
- Make (optional, for building)

### Build Instructions
```sh
git clone https://github.com/IonascuAndrey/falloc.git
cd falloc
make
```

To use it in your project:
```sh
gcc -o my_program my_program.c -L./build -lfalloc
```

## Usage

Include the library in your C project:

```c
#include "falloc.h"

int main() {
    void *ptr = fmalloc(100);  // Allocate 100 bytes
    if (!ptr) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    ptr = frealloc(ptr, 200);  // Resize memory
    ffree(ptr);  // Free memory

    return 0;
}
```

## API Reference

### `void *fmalloc(size_t size)`
Allocates `size` bytes of memory and returns a pointer to it.

### `void *frealloc(void *ptr, size_t new_size)`
Reallocates memory, resizing the block pointed to by `ptr` to `new_size` bytes.

### `void ffree(void *ptr)`
Frees the allocated memory pointed to by `ptr`.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

