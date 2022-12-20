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

list_t *first = NULL;
list_t *last = NULL;

list_t *create_block(size_t size);
list_t *get_free_block(list_t* current, size_t size);
void merge_adjacent(list_t* current);

list_t *create_block(size_t size) {
    list_t *block = sbrk(size + sizeof(list_t));

    if (block == (void *) -1)
        return NULL;

    block->size = size;
    block->next = NULL;

    return block;
}

list_t *get_free_block(list_t* current, size_t size) {
    while (current && !(current->free && current->size >= size))
        current = current->next;

    return current;
}

void merge_adjacent(list_t* header) {
    list_t *prev = first;
    list_t *next = header->next;

    while (prev && prev->next != header)
        prev = prev->next;

    if (next && next->free) {
        header->next = next->next;
        header->size += sizeof(list_t) + next->size;
        if (last == next)
            last = header;
    }

    if (prev && prev->free) {
        prev->next = header->next;
        prev->size += sizeof(list_t) + header->size;
        if (last == header)
            last = prev;
        header = prev;
    }
}

void *malloc(size_t size);
void *calloc(size_t n, size_t size);
void *realloc(void *src, size_t size);
void free(void *ptr);

void *malloc(size_t size) {
    list_t *header = NULL;
    if (size == 0)
        return NULL;

    if (first) {
        header = get_free_block(first, size);
    }

    if (!header) {
        header = create_block(size);
        if (!header)
            return NULL;

        if (last)
            last->next = header;
        else
            first = header;
        last = header;
    }

    header->free = false;
    return (header + 1);
}

void *calloc(size_t n, size_t size) {
    void *block = malloc(n * size);

    if (block)
        memset(block, 0, n * size);

    return block;
}

void *realloc(void *src, size_t size) {
    if (size == 0)
        return NULL;

    list_t *src_header;
    if (src) {
        src_header = (list_t*) src - 1;
        if (src_header->size >= size)
            return src;
    }

    void *block = malloc(size);

    if (!src)
        return block;

    assert(src_header->free == false);
    assert(src_header->size > 0);

    if (block) {
        size = size > src_header->size ? src_header->size : size;
        memmove(block, src, size);
        free(src);
    }

    return block;
}

void free(void *ptr) {
    if (!ptr)
        return;

    list_t *header = (list_t*) ptr - 1;
    assert(header->free == false);
    header->free = true;

    // merge_adjacent(header);
}