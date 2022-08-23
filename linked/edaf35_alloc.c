#include "edaf35_alloc.h"

#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct list_t list_t;

struct list_t {
    size_t size;
    list_t* next;
    bool free;
};

list_t* first = NULL;
list_t* last = NULL;

list_t* create_block(size_t size);
list_t* get_free_block(list_t* current, size_t size);

list_t* create_block(size_t size) {
    list_t* block = sbrk(size + sizeof(list_t));
    
    if (!block)
        return NULL;
    
    block->size = size;
    block->next = NULL;
    
    if (last) {
        last->next = block;
        last = block;
    } else {
        first = block;
        last = block;
    }

    return block;
}

list_t* get_free_block(list_t* current, size_t size) {
    while (current && !(current->free && current->size >= size)) 
        current = current->next;

    if (!current) 
        return create_block(size);

    return current;
}

void* malloc(size_t size) {
    list_t* header;
    if (size == 0) 
        return NULL;

    if (!first)
        header = create_block(size);
    else
        header = get_free_block(first, size);
    
    if (!header) {
        assert(false);
        return NULL;
    }

    header->free = false;
    return (header + 1);
}

void* calloc(size_t n, size_t size) {
    void* block = malloc(n * size);
    
    if (block) 
        memset(block, 0, n * size);

    return block;
}

void* realloc(void* src, size_t size) {
    void* block = malloc(size);
    
    if (!src)
        return block;
    
    list_t* src_header = (list_t*) src - 1;
    assert(src_header->free == false);
    assert(src_header->size > 0);
    size = size > src_header->size ? src_header->size : size;

    if (block) {
        memmove(block, src, size);
        free(src);
    }

    return block;
}

void free(void* ptr) {
    if (!ptr)
        return;

    list_t* header = (list_t*) ptr - 1;
    assert(header->free == false);
    header->free = true;
}