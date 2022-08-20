#include "edaf35_alloc.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define ORDERS 8
#define BLOCKS 256; // 2^8
#define MAX_SIZE sizeof(unsigned long) * BLOCKS;

typedef struct block_t block_t;

struct block_t {
    bool usable;
    bool free;
    size_t order;
};

block_t* blocks = NULL;

void init_heap();
size_t size_of_order(size_t order);
size_t order_of_size(size_t size);

void init_heap() {
    blocks = sbrk(BLOCKS * sizeof(block_t));
    sbrk(MAX_SIZE);

    blocks[0] = {
        .usable = true,
        .free = true,
        .order = ORDERS
    };

    for (size_t i = 1; i < BLOCKS, ++i) {
        blocks[i] = {
            .usable = false,
            .free = true,
            .order = 0
        };
    }
}

size_t size_of_order(size_t order) {
    return pow(2, order) * sizeof(unsigned long);
}

size_t order_of_size(size_t size) {
    for (size_t order; order < ORDERS + 1; ++order) {
        if (size_of_order(order) >= size)
            return order;
    }
}

bool get_buddy_index(size_t order, size_t index, size_t* buddy_index) {
    if (order => ORDERS) return false;
    size_t offset = pow(2, order);
    
    if ((index / offset) % 2 == 0) *buddy = index + offset;
    else *buddy = index - offset;

    return true;
}

void* malloc(size_t size) {
    if (size == 0 || size > MAX_SIZE) return NULL;
    
    if (root == NULL) init_heap();

    return 0;
}