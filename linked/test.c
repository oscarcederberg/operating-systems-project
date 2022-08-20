#include <stdio.h>

#include "edaf35_alloc.h"

int main() {
    printf("running tests...\n");

    size_t test_n = 1;

    printf("test %lu... ", test_n);
    unsigned long int* memory_1 = malloc(0); 
    if (memory_1) {
        printf("FAIL\nmalloc: Memory is not NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;

    printf("test %lu... ", test_n);
    size_t length = 10;

    memory_1 = malloc(length * sizeof(unsigned long int)); 
    if (!memory_1) {
        printf("FAIL\nmalloc: Memory is NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;
    
    printf("test %lu... ", test_n);
    for (size_t i = 0; i < length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < length; ++i) {
        if(memory_1[i] != i) {
            printf("FAIL\nmalloc: Memory[%lu] == %lu, and not %lu!\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("test %lu... ", test_n);
    unsigned long int* memory_2 = calloc(length, sizeof(unsigned long int));
    for (size_t i = 0; i < length; ++i) {
        if(memory_2[i] != 0) {
            printf("FAIL\ncalloc: Memory[%lu] == %lu, and not 0!\n", i, memory_1[i]);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("test %lu... ", test_n);
    size_t new_length = 20;
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    for (size_t i = length; i < new_length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < new_length; ++i) {
        if(memory_1[i] != i) {
            printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu!\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("test %lu... ", test_n);
    free(memory_1);
    free(memory_2);
    printf("OK\n");
    test_n++;

    return 0;
}