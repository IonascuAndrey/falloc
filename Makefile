CC = gcc
CFLAGS = -Wall -Werror -fPIC
LIBRARY = libfalloc.a   # Static library name

SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(LIBRARY) main

# Build static library
$(LIBRARY): $(OBJ_FILES)
	ar rcs $@ $(OBJ_FILES)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Link the main program with the static library
main: main.c $(LIBRARY)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -Iinclude -L. main.c -lfalloc -o main

# Clean object files and the library
clean:
	rm -f $(OBJ_DIR)/*.o main $(LIBRARY)

.PHONY: all clean
