#include "alloc.h"

int main() {
    size_t length = 10;

    unsigned long int* memory_1 = malloc(length * sizeof(unsigned long int)); 
    if (!memory_1) {
        printf("malloc: Memory is NULL\n");
        return 1;
    }
    for (size_t i = 0; i < length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < length; ++i) {
        if(memory_1[i] != i) {
            printf("malloc: Memory[%lu] == %lu, and not %lu!\n", i, memory_1[i], i);
            return 1;
        }
    }

    unsigned long int* memory_2 = calloc(length, sizeof(unsigned long int));
    for (size_t i = 0; i < length; ++i) {
        if(memory_2[i] != 0) {
            printf("calloc: Memory[%lu] == %lu, and not 0!\n", i, memory_1[i]);
            return 1;
        }
    }

    size_t new_length = 20;
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    for (size_t i = length; i < new_length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < new_length; ++i) {
        if(memory_1[i] != i) {
            printf("realloc: Memory[%lu] == %lu, and not %lu!\n", i, memory_1[i], i);
            return 1;
        }
    }

    free(memory_1);
    free(memory_2);

    return 0;
}