#include <stdio.h>

#include "edaf35_alloc.h"

int main() {
    printf("running tests...\n");

    size_t test_n = 1;

    printf("%lu: malloc size 0\ntesting....", test_n);
    unsigned long int* memory_1 = malloc(0); 
    if (memory_1) {
        printf("FAIL\nmalloc: Memory is not NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;

    printf("%lu: malloc size 10\ntesting... ", test_n);
    size_t length = 10;
    memory_1 = malloc(length * sizeof(unsigned long int)); 
    if (!memory_1) {
        printf("FAIL\nmalloc: Memory is NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: malloc storing values\ntesting... ", test_n);
    for (size_t i = 0; i < length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < length; ++i) {
        if(memory_1[i] != i) {
            printf("FAIL\nmalloc: Memory[%lu] == %lu, and not %lu\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("%lu: calloc n 0\ntesting... ", test_n);
    unsigned long int* memory_2 = calloc(0, sizeof(unsigned long int));
    if(memory_2) {
        printf("FAIL\ncalloc: Memory is not NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: calloc n 10\ntesting... ", test_n);
    memory_2 = calloc(length, sizeof(unsigned long int));
    if(!memory_2) {
        printf("FAIL\ncalloc: Memory is NULL\n");
        return 1;
    }
    for (size_t i = 0; i < length; ++i) {
        if(memory_2[i] != 0) {
            printf("FAIL\ncalloc: Memory[%lu] == %lu, and not 0\n", i, memory_1[i]);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("%lu: realloc src null or size 0\ntesting... ", test_n);
    unsigned long int* memory_3 = realloc(NULL, length);
    if (!memory_3) {
        printf("FAIL\nrealloc: Memory is NULL\n");
        return 1;
    }
    memory_3 = realloc(memory_3, 0);
    if (memory_3) {
        printf("FAIL\nrealloc: Memory is not NULL\n");
        return 1;
    }
    printf("OK\n");
    test_n++;

    printf("%lu: realloc from size 10 to 20\ntesting... ", test_n);
    size_t new_length = 20;
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    for (size_t i = length; i < new_length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < new_length; ++i) {
        if(memory_1[i] != i) {
            printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: realloc to 100000000 two times\ntesting... ", test_n);
    new_length = 100000000;
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    memory_2 = realloc(memory_2, new_length * sizeof(unsigned long int));
    for (size_t i = 0; i < new_length; ++i) {
        memory_1[i] = i;
        memory_2[new_length - i - 1] = i;
    }
    for (size_t i = 0; i < new_length; ++i) {
        if(memory_1[i] != i) {
            printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu\n", i, memory_1[i], i);
            return 1;
        }
        if(memory_2[i] != new_length - i - 1) {
            printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu\n", i, memory_2[i], new_length - i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: freeing NULL\ntesting... ", test_n);
    free(NULL);
    printf("OK\n");
    test_n++;

    printf("%lu: freeing memory\ntesting... ", test_n);
    free(memory_1);
    free(memory_2);
    printf("OK\n");
    test_n++;

    return 0;
}