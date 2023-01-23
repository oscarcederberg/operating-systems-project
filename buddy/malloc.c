#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_ORDER (24)
#define BLOCKS (1UL << MAX_ORDER)
#define MIN_BLOCK_SIZE (128)
#define MAX_SIZE (BLOCKS * MIN_BLOCK_SIZE)

typedef struct block_t block_t;

struct block_t {
    bool free;
    size_t order;
    block_t *prev;
    block_t *next;
};

block_t *head = NULL;
size_t free_blocks[MAX_ORDER + 1] = {0};

void init_heap();
size_t max_size_of_order(size_t order);
size_t min_order_for_size(size_t size);
bool merge(block_t *block);
block_t *split(size_t of_order);
block_t *get_buddy_block(block_t *block);
block_t *get_free_block(size_t order);

void init_heap() {
    head = sbrk(MAX_SIZE);
    if (head == (void *) -1) {
        head = NULL;
        return;
    }

    free_blocks[MAX_ORDER] = 1;
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

bool merge(block_t *block) {
    size_t order = block->order;

    block_t *buddy = get_buddy_block(block);
    if (!buddy || buddy->order != order || !buddy->free) {
        return false;
    }

    block_t *left, *right;
    if (block < buddy) {
        left = block;
        right = buddy;
    } else {
        left = buddy;
        right = block;
    }

    left->order = order + 1;
    right->order = order + 1;
    left->next = right->next;
    right->next = NULL;
    right->prev = NULL;
    free_blocks[order] -= 2;
    free_blocks[order + 1] += 1;

    merge(block); // Merge until merge is not possible

    return true;
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

    block_t *buddy = (block_t *) (((void *) block) + (1UL << block->order) * MIN_BLOCK_SIZE + sizeof(block_t));
    buddy->free = true;
    buddy->order = order - 1;
    buddy->next = block->next;
    buddy->prev = block;
    block->next = buddy;

    free_blocks[order] -= 1;
    free_blocks[order - 1] += 2;

    return block;
}

block_t *get_buddy_block(block_t *block) {
    size_t order = block->order;

    if (order >= MAX_ORDER) {
        return NULL;
    }

    block_t *buddy;

    if ((((unsigned long int) block) - ((unsigned long int) head)) / (1UL << order * MIN_BLOCK_SIZE)) {
        buddy = block->prev;
    } else {
        buddy = block->next;
    }

    return buddy;
}

block_t *get_free_block(size_t of_order) {
    block_t *block = head;

    if (!free_blocks[of_order]) {
        return split(of_order + 1);
    }

    while (block) {
        if (block->free && block->order == of_order) {
            return block;
        }
        block = block->next;
    }

    return NULL;
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
    free_blocks[order] -= 1;
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
    free_blocks[block->order] += 1;

    merge(block);
}