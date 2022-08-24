#include "edaf35_alloc.h"

#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//#define DEBUG_MERGE
//#define SPLIT
//#define DEBUG_SPLIT

typedef struct list_t list_t;

struct list_t {
    size_t size;
    list_t* next;
    bool free;
};

list_t* first = NULL;
// TODO: maybe remove, less bookeeping?
list_t* last = NULL;

list_t* create_block(size_t size);
list_t* get_free_block(list_t* current, size_t size);
void merge_adjacent(list_t* current);
void split_block(list_t* header, size_t size);

list_t* create_block(size_t size) {
    list_t* block = sbrk(size + sizeof(list_t));
    
    if (!block)
        return NULL;
    
    block->size = size;

    return block;
}

list_t* get_free_block(list_t* current, size_t size) {
    while (current && !(current->free && current->size >= size)) 
        current = current->next;

    if (!current) 
        return create_block(size);

    return current;
}

void merge_adjacent(list_t* header) {
    list_t* prev = first;
    list_t* next = header->next;

    if (first == header)
        prev = NULL;
    
    while (prev && prev->next != header)
        prev = prev->next;

    #ifdef DEBUG_MERGE
    printf("\ncurr:%lu\n", header->size);
    #endif
    if (next && next->free) {
        #ifdef DEBUG_MERGE
        printf("next:%lu\n", next->size);
        printf("   +:%lu\n", sizeof(list_t));
        #endif

        header->next = next->next;
        header->size += sizeof(list_t) + next->size;
        if (last == next)
            last = header;
    }

    if (prev && prev->free) {
        #ifdef DEBUG_MERGE
        printf("prev:%lu\n", prev->size);
        printf("   +:%lu\n", sizeof(list_t));
        #endif

        prev->next = header->next;
        prev->size = sizeof(list_t) + header->size;
        if (last == header)
            last = prev;
        header = prev;
    }

    #ifdef DEBUG_MERGE
    printf("after:%lu\n", header->size);
    #endif
}

void split_block(list_t* header, size_t size) {
    #ifdef DEBUG_SPLIT
    printf("\nheader: %p\n%lu ->\n%lu\n", (void*) header, header->size, size);
    #endif

    // TODO: i don't think this is correct at all
    list_t* split = (void*)(header + sizeof(list_t) + size);
    split->free = true;
    split->size = header->size - size - sizeof(list_t);
    header->size = size;
    split->next = header->next;
    header->next = split;

    #ifdef DEBUG_SPLIT
    printf("split: %p\n%lu\n", (void*) split, split->size);
    printf("diff: %ld", ((void *) split) - ((void*) header));
    #endif
}

void* malloc(size_t size) {
    list_t* header;
    if (size == 0) 
        return NULL;

    if (!first) {
        header = create_block(size);
        if (last) {
            last->next = header;
            last = header;
        } else {
            first = header;
            last = header;
        }
    } else {
        header = get_free_block(first, size);
        #ifdef SPLIT
        if (header->size > sizeof(list_t) + size)
            split_block(header, size);
        #endif
    }
    
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
    if (size == 0)
        return NULL;

    list_t* src_header;
    if (src) {
        src_header = (list_t*) src - 1;
        if (src_header->size >= size)
            return src;
    }

    void* block = malloc(size);
    
    if (!src)
        return block;
    
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

    merge_adjacent(header);
}