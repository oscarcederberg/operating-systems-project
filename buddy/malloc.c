#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ORDER (24)
#define BLOCKS (1L << MAX_ORDER)
#define MAX_SIZE (BLOCKS * sizeof(unsigned long int))

typedef struct block_t block_t;

struct block_t {
    bool usable;
    bool free;
    size_t order;
};

block_t blocks[BLOCKS];
void *heap;

void init_heap();
size_t size_of_order(size_t order);
size_t order_of_size(size_t size);
bool split(size_t order, size_t *index);
void merge(size_t index);
bool get_buddy_index(size_t order, size_t index, size_t *buddy);
bool get_block_index(size_t order, size_t *index);

void init_heap() {
    heap = sbrk(MAX_SIZE);
    if (heap == (void *) -1) {
        assert(false);
    }

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

size_t size_of_order(size_t order) {
    return pow(2, order) * sizeof(unsigned long int);
}

size_t order_of_size(size_t size) {
    for (size_t order = 0; order < MAX_ORDER + 1; ++order) {
        if (size_of_order(order) >= size) {
            return order;
        }
    }
    assert(false);
    return 0;
}

bool split(size_t order, size_t *index) {
    size_t num_blocks_with_order = pow(2, (MAX_ORDER - order));
    size_t offset = pow(2, order);
    block_t *block = NULL;

    for (size_t i = 0; i < num_blocks_with_order; i++) {
        block = &(blocks[i * offset]);
        if (block->order != order || !block->usable || !block->free) {
            continue;
        }

        block->usable = true;
        block->free = true;
        block->order = order - 1;

        size_t buddy_index;
        get_buddy_index(order, i, &buddy_index);
        blocks[buddy_index].usable = true;
        blocks[buddy_index].free= true;
        blocks[buddy_index].order = order - 1;

        (*index) = i * offset;
        return true;
    }

    if (order == MAX_ORDER - 1) {
        return false;
    }

    return split(order + 1, index);
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

bool get_block_index(size_t order, size_t *index) {
    size_t num_blocks_with_order = pow(2, (MAX_ORDER - order));
    size_t offset = pow(2, order);
    block_t *block = NULL;

    for (size_t i = 0; i < num_blocks_with_order; i++) {
        block = &(blocks[i * offset]);
        if (block->order == order && block->usable && block->free) {
            (*index) = i * offset;
            return true;
        }
    }

    if (order == MAX_ORDER) {
        return false;
    }

    return split(order + 1, index);
}

void *_malloc(size_t size, size_t *index);
void *malloc(size_t size);
void *calloc(size_t n, size_t size);
void *realloc(void *src, size_t size);
void free(void *ptr);

void *_malloc(size_t size, size_t *index) {
    if (size == 0 || size > MAX_SIZE) return NULL;

    if (blocks == NULL) init_heap();

    size_t order = order_of_size(size);

    if (!get_block_index(order, index)) {
        return NULL;
    }

    blocks[(*index)].free = false;
    blocks[(*index)].usable = true;
    return ((char *) heap + ((*index) * sizeof(unsigned long int)));
}

void *malloc(size_t size) {
    size_t index;
    return _malloc(size, &index);
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

    size_t src_index;
    if (src) {
        src_index = (size_t) ((char *)((src - heap) / sizeof(unsigned long int)));
        if (blocks[src_index].order >= order_of_size(size))
            return src;
    }

    size_t new_index;
    void *dst = _malloc(size, &new_index);

    if (!src)
        return dst;

    if (dst) {
        size_t order = blocks[src_index].order;

        size = size > size_of_order(order) ? size_of_order(order) : size;
        memmove(dst, src, size);
        free(src);
    }

    return dst;
}

void free(void *ptr) {
    if (!ptr)
        return;

    size_t index = (size_t) ((char *)((ptr - heap) / sizeof(unsigned long int)));
    assert(blocks[index].free == false);
    blocks[index].free = true;

    merge(index);
}