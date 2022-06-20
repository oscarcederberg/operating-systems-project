#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define BLOCKS 256; // 2^8
#define MAX_SIZE sizeof(unsigned long) * BLOCKS;

typedef struct node_t node_t;

struct node_t {
    size_t size;
    void* data;
    bool free;
    node_t* left;
    node_t* right;
    node_t* buddy;
};

node_t* root = NULL;

void* malloc(size_t size);
void* calloc(size_t n, size_t size);
void* realloc(void* src, size_t size);
void free(void* ptr);

void* malloc(size_t size) {
    if(size == 0 || size > MAX_SIZE) return NULL;
    
    if(root == NULL) {
        void* data = sbrk(MAX_SIZE);
        new_root = {
            .size = MAX_SIZE,
            .data = data,
            .free = true,
            .left = NULL,
            .right = NULL,
            .buddy = NULL
        };
        root = &new_root;
    }

    return root->data;
}