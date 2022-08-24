#include <stdio.h>
#include <limits.h>

#include "edaf35_alloc.h"

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned rand() {
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

void shuffle(unsigned long int** array, size_t n) {
    if (n == 0)
        return;
    
    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (UINT_MAX / (n - i) + 1);
        unsigned long int* t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

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

    size_t length = 10;
    printf("%lu: malloc size %lu\ntesting... ", test_n, length);
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
        if (memory_1[i] != i) {
            printf("FAIL\nmalloc: Memory[%lu] == %lu, and not %lu\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("%lu: calloc n 0 or size 0\ntesting... ", test_n);
    unsigned long int* memory_2 = calloc(0, sizeof(unsigned long int));
    if (memory_2) {
        printf("FAIL\ncalloc: Memory is not NULL\n");
        return 1;
    }
    free(memory_2);
    memory_2 = calloc(0, sizeof(unsigned long int));
    if (memory_2) {
        printf("FAIL\ncalloc: Memory is not NULL\n");
        return 1;
    }
    free(memory_2);
    printf("OK\n");
    test_n++;
    
    printf("%lu: calloc n %lu\ntesting... ", test_n, length);
    memory_2 = calloc(length, sizeof(unsigned long int));
    if (!memory_2) {
        printf("FAIL\ncalloc: Memory is NULL\n");
        return 1;
    }
    for (size_t i = 0; i < length; ++i) {
        if (memory_2[i] != 0) {
            printf("FAIL\ncalloc: Memory[%lu] == %lu, and not 0\n", i, memory_1[i]);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;

    printf("%lu: realloc src NULL or size 0\ntesting... ", test_n);
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

    size_t new_length = 20;
    printf("%lu: realloc from size %lu to %lu\ntesting... ", test_n, length, new_length);
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    for (size_t i = length; i < new_length; ++i) {
        memory_1[i] = i;
    }
    for (size_t i = 0; i < new_length; ++i) {
        if (memory_1[i] != i) {
            printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu\n", i, memory_1[i], i);
            return 1;
        }
    }
    printf("OK\n");
    test_n++;
    
    size_t iterations = 1000;
    printf("%lu: malloc, storing, and freeing %lu times\ntesting... ", test_n, iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        free(memory_1);
        free(memory_2);
        memory_1 = malloc(new_length * sizeof(unsigned long int));
        memory_2 = malloc(new_length * sizeof(unsigned long int));
        for (size_t j = 0; j < new_length; ++j) {
            memory_1[j] = j;
            memory_2[new_length - j - 1] = j;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != j) {
                printf("FAIL\nmalloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], j, i);
                return 1;
            }
            if (memory_2[j] != new_length - j - 1) {
                printf("FAIL\nmalloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], new_length - j, i);
                return 1;
            }
        }
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: calloc and freeing %lu times\ntesting... ", test_n, iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        free(memory_1);
        free(memory_2);
        memory_1 = calloc(new_length, sizeof(unsigned long int));
        memory_2 = calloc(new_length, sizeof(unsigned long int));
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != 0) {
                printf("FAIL\ncalloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], 0lu, i);
                return 1;
            }
            if (memory_2[j] != 0) {
                printf("FAIL\ncalloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], 0lu, i);
                return 1;
            }
        }
    }
    printf("OK\n");
    test_n++;
    
    printf("%lu: realloc, storing, and freeing %lu times\ntesting... ", test_n, iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
        memory_2 = realloc(memory_2, new_length * sizeof(unsigned long int));
        for (size_t j = 0; j < new_length; ++j) {
            memory_1[j] = j;
            memory_2[new_length - j - 1] = j;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != j) {
                printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], j, i);
                return 1;
            }
            if (memory_2[j] != new_length - j - 1) {
                printf("FAIL\nrealloc: Memory[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], new_length - j, i);
                return 1;
            }
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

    size_t allocations = 10000;
    new_length = 10000;
    printf("%lu: malloc, store, and free %lu arrays randomly\ntesting... ", test_n, allocations);
    unsigned long int** memories = malloc(allocations * sizeof(unsigned long int*));
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        memories[i] = malloc(new_length * sizeof(unsigned long int));
        for (size_t j = 0; j < new_length; ++j) {
            if (i % 2)
                memories[i][j] = j;
            else
                memories[i][new_length - j - 1] = j;
        }
    }
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        for (size_t j = 0; j < new_length; ++j) {
            if (i % 2 && memories[i][j] != j) {
                printf("FAIL\nmalloc: Memory[%lu][%lu] == %lu, and not %lu\n", i, j, memories[i][j], j);
                return 1;
            }
            if (!(i % 2) && memories[i][j] != new_length - j - 1) {
                printf("FAIL\nmalloc: Memory[%lu][%lu] == %lu, and not %lu\n", i, j, memories[i][j], new_length - j);
                return 1;
            }
        }
    }
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        free(memories[i]);
    }
    free(memories);
    printf("OK\n");
    test_n++;
    
    printf("%lu: calloc, store, and free %lu arrays randomly\ntesting... ", test_n, allocations);
    memories = calloc(allocations, sizeof(unsigned long int*));
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        memories[i] = calloc(new_length, sizeof(unsigned long int));
        for (size_t j = 0; j < new_length; ++j) {
            if (i % 2)
                memories[i][j] = j;
        }
    }
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        for (size_t j = 0; j < new_length; ++j) {
            if (i % 2 && memories[i][j] != j) {
                printf("FAIL\ncalloc: Memory[%lu][%lu] == %lu, and not %lu\n", i, j, memories[i][j], j);
                return 1;
            }
            if (!(i % 2) && memories[i][j] != 0) {
                printf("FAIL\ncalloc: Memory[%lu][%lu] == %lu, and not %lu\n", i, j, memories[i][j], 0lu);
                return 1;
            }
        }
    }
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        free(memories[i]);
    }
    free(memories);
    printf("OK\n");
    test_n++;

    return 0;
}