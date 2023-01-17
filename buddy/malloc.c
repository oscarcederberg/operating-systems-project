#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ORDER (24)
#define BLOCKS (1UL << MAX_ORDER)
#define MAX_SIZE (BLOCKS * sizeof(unsigned long int))

typedef struct block_t block_t;

struct block_t {
    bool usable;
    bool free;
    size_t order;
};

block_t blocks[BLOCKS];
bool heap_initialized = false;
void *heap;

void init_heap();
size_t max_size_of_order(size_t order);
size_t min_order_for_size(size_t size);
bool split(size_t order, size_t *index);
void merge(size_t index);
bool get_buddy_index(size_t order, size_t index, size_t *buddy);
bool get_free_index(size_t order, size_t *index);

void init_heap() {
    heap = sbrk(MAX_SIZE);
    if (heap == (void *) -1) {
        assert(false);
    }
    heap_initialized = true;

    blocks[0] = (block_t) {
        .usable = true,
        .free = true,
        .order = MAX_ORDER
    };

    for (size_t i = 1; i < BLOCKS; ++i) {
        blocks[i] = (block_t) {
            .usable = false,
            .free = true,
            .order = MAX_ORDER
        };
    }
}

size_t max_size_of_order(size_t order) {
    return (1UL << order) * sizeof(unsigned long int);
}

size_t min_order_for_size(size_t size) {
    for (size_t order = 0; order <= MAX_ORDER; order++) {
        if (max_size_of_order(order) >= size) {
            return order;
        }
    }
    assert(false);
    return 0;
}

bool split(size_t order, size_t *index) {
    block_t *block = NULL;

    if (order == MAX_ORDER) {
        block = &(blocks[0]);
        if (block->order != MAX_ORDER || !block->free || !block->usable) {
            return false;
        }
    }

    if (get_free_index(order, index) || split(order + 1, index)) {
        block = &(blocks[(*index)]);
    } else {
        return false;
    }

    block->usable = true;
    block->free = true;
    block->order = order - 1;

    size_t buddy_index;
    get_buddy_index(order - 1, *index, &buddy_index);

    block = &(blocks[buddy_index]);
    block->usable = true;
    block->free = true;
    block->order = order - 1;
    return true;
}

void merge(size_t index) {
    size_t order = blocks[index].order;
    size_t buddy;

    if (!get_buddy_index(order, index, &buddy)) {
        return;
    }

    if (blocks[buddy].order == order && blocks[buddy].free) {
        blocks[index].order = order + 1;
        blocks[buddy].usable = false;
        blocks[buddy].order = order + 1;
    }
}

bool get_buddy_index(size_t order, size_t index, size_t* buddy) {
    if (order >= MAX_ORDER) {
        return false;
    }

    size_t offset = pow(2, order);

    if ((index / offset) % 2 == 0) {
        *buddy = index + offset;
    } else {
        *buddy = index - offset;
    }

    return true;
}

bool get_free_index(size_t order, size_t *index) {
    size_t num_blocks_with_order = 1 << (MAX_ORDER - order);
    size_t offset = 1 << order;
    block_t *block = NULL;

    for (size_t i = 0; i < num_blocks_with_order; i++) {
        block = &(blocks[i * offset]);
        if (block->order == order && block->usable && block->free) {
            (*index) = i * offset;
            return true;
        }
    }

    return false;
}

void *malloc(size_t size);
void *calloc(size_t n, size_t size);
void *realloc(void *src, size_t size);
void free(void *ptr);

void *malloc(size_t size) {
    size_t index;

    if (size == 0 || size > MAX_SIZE) {
        return NULL;
    }

    if (!heap_initialized) {
        init_heap();
    }

    size_t order = min_order_for_size(size);

    if (!get_free_index(order, &index) && !split(order - 1, &index)) {
        return NULL;
    }

    blocks[index].usable = true;
    blocks[index].free = false;
    return ((char *) heap + (index * sizeof(unsigned long int)));
}

void *calloc(size_t n, size_t size) {
    void *block = malloc(n * size);

    if (block) {
        memset(block, 0, n * size);
    }

    return block;
}

void *realloc(void *src, size_t size) {
    if (size == 0) {
        return NULL;
    }

    size_t src_index;
    if (src) {
        src_index = (src - heap) / sizeof(unsigned long int);
        if (blocks[src_index].order == min_order_for_size(size)) {
            return src;
        }
    }

    void *dst = malloc(size);

    if (!src || !dst) {
        return dst;
    }

    size_t src_size = max_size_of_order(blocks[src_index].order);
    size = size > src_size ? src_size : size;
    memmove(dst, src, size * sizeof(unsigned long int));
    free(src);

    return dst;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    size_t index = (ptr - heap) / sizeof(unsigned long int);
    assert(blocks[index].free == false);
    blocks[index].free = true;

    merge(index);
}