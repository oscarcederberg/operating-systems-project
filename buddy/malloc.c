#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ORDER (22)
#define BLOCKS (1UL << MAX_ORDER)
#define MIN_BLOCK_SIZE (64)
#define MAX_SIZE (BLOCKS * MIN_BLOCK_SIZE)

typedef struct block_t block_t;

struct block_t {
    bool free;
    size_t order;
    block_t *prev;
    block_t *next;
};

block_t *head = NULL;

void init_heap();
size_t max_size_of_order(size_t order);
size_t min_order_for_size(size_t size);
void merge(block_t *block);
block_t *split(size_t of_order);
block_t *get_buddy_block(block_t *block);
block_t *get_free_block(size_t order);

void init_heap() {
    head = sbrk(MAX_SIZE);
    if (head == (void *) -1) {
        assert(false);
    }

    head->free = true;
    head->order = MAX_ORDER;
    head->prev = NULL;
    head->next = NULL;
}

size_t max_size_of_order(size_t order) {
    return (1UL << order) * MIN_BLOCK_SIZE - sizeof(block_t);
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

void merge(block_t *block) {
    size_t order = block->order;

    block_t *buddy = get_buddy_block(block);
    if (!buddy || buddy->order != order || !buddy->free) {
        return;
    }

    block_t *left, *right;
    if (block > buddy) {
        left = block;
        right = buddy;
    } else {
        left = buddy;
        right = block;
    }

    left->order = order + 1;
    left->next = right->next;
}

block_t *split(size_t order) {
    if (order > MAX_ORDER) {
        return false;
    }

    block_t *block = get_free_block(order);
    if (!block) {
        return NULL;
    }

    block->free = true;
    block->order = order - 1;

    block_t *buddy = (block_t *) (((void *) block) + (1UL << block->order) * MIN_BLOCK_SIZE);
    buddy->free = true;
    buddy->order = order - 1;
    buddy->next = block->next;
    buddy->prev = block;
    block->next = buddy->next;

    return block;
}

block_t *get_buddy_block(block_t *block) {
    size_t order = block->order;

    if (order >= MAX_ORDER) {
        return NULL;
    }

    block_t *buddy;

    if (((unsigned long int) block) / (1UL << order * MIN_BLOCK_SIZE)) {
        buddy = block->prev;
    } else {
        buddy = block->next;
    }

    if (buddy->order != order) {
        return NULL;
    }

    return buddy;
}

block_t *get_free_block(size_t of_order) {
    block_t *block = head;

    while (block) {
        if (block->free && block->order == of_order) {
            return block;
        }
        block = block->next;
    }

    return split(of_order + 1);
}

void *malloc(size_t size);
void *calloc(size_t n, size_t size);
void *realloc(void *src, size_t size);
void free(void *ptr);

void *malloc(size_t size) {
    if (size == 0 || size > MAX_SIZE) {
        return NULL;
    }

    if (!head) {
        init_heap();
    }

    size_t order = min_order_for_size(size);

    block_t *block = get_free_block(order);
    if (!block) {
        return NULL;
    }

    block->free = false;
    return (block + 1);
}

void *calloc(size_t n, size_t size) {
    void *data = malloc(n * size);

    if (data) {
        memset(data, 0, n * size);
    }

    return data;
}

void *realloc(void *src, size_t size) {
    if (size == 0) {
        return NULL;
    }

    block_t *src_block;
    if (src) {
        src_block = ((block_t *) src) - 1;
        if (src_block->order == min_order_for_size(size)) {
            return src;
        }
    }

    void *dst = malloc(size);

    if (!src || !dst) {
        return dst;
    }

    size_t src_size = max_size_of_order(src_block->order);
    size = size > src_size ? src_size : size;
    memmove(dst, src, size);
    free(src);

    return dst;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    block_t *block = ((block_t *) ptr) - 1;
    assert(block->free == false);
    block->free = true;

    merge(block);
}