#ifndef ALLOC_H
#define ALLOC_H

#include <unistd.h>

void* malloc(size_t size);
void* calloc(size_t n, size_t size);
void* realloc(void* src, size_t size);
void free(void* ptr);

#endif