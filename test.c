#include <stdio.h>
#include <limits.h>

#include "malloc.c"

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned my_rand() {
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    return lfsr =  (lfsr >> 1) | (bit << 15);
}

void shuffle(unsigned long int** array, size_t n) {
    if (n == 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + my_rand() / (UINT_MAX / (n - i) + 1);
        unsigned long int* t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

bool overlaps(unsigned long int *array_1, unsigned long int *array_2, size_t size_1, size_t size_2) {
    if (array_1 >= array_2 && array_1 <= array_2 + size_2) {
        return true;
    } else if (array_2 >= array_1 && array_2 <= array_1 + size_1) {
        return true;
    }

    return false;
}

int main() {
    printf("running tests...\n");

    printf("1: malloc size 0\ntesting....");
    unsigned long int* memory_1 = malloc(0);
    if (memory_1) {
        printf("WARNING\nmalloc: Memory is not NULL\n");
    } else {
        printf("OK\n");
    }
    free(memory_1);

    size_t length = 10;
    printf("2: malloc size %lu\ntesting... ", length);
    memory_1 = malloc(length * sizeof(unsigned long int));
    if (!memory_1) {
        printf("FAIL\nmalloc: Memory is NULL\n");
        return 1;
    }
    printf("OK\n");

    printf("3: malloc storing values\ntesting... ");
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

    printf("4: calloc size 0\ntesting... ");
    unsigned long int* memory_2 = calloc(0, sizeof(unsigned long int));
    if (memory_2) {
        printf("WARNING\ncalloc: Memory is not NULL\n");
    } else {
        printf("OK\n");
    }
    free(memory_2);

    printf("5: calloc n 0\ntesting... ");
    memory_2 = calloc(0, sizeof(unsigned long int));
    if (memory_2) {
        printf("WARNING\ncalloc: Memory is not NULL\n");
    } else {
        printf("OK\n");
    }
    free(memory_2);

    printf("6: calloc n %lu\ntesting... ", length);
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

    printf("7: realloc src NULL or size 0\ntesting... ");
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

    size_t new_length = 20;
    printf("8: realloc from size %lu to %lu\ntesting... ", length, new_length);
    memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
    if (!memory_1) {
        printf("FAIL\nrealloc: Memory is NULL\n");
        return 1;
    }
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

    size_t iterations = 100;
    printf("9: malloc, storing, and freeing %lu times\ntesting... ", iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        free(memory_1);
        free(memory_2);
        memory_1 = malloc(new_length * sizeof(unsigned long int));
        if (!memory_1) {
            printf("FAIL\nmalloc: Memory_1 was null after malloc at iteration %lu\n", i);
            return 1;
        }
        memory_2 = malloc(new_length * sizeof(unsigned long int));
        if (!memory_2) {
            printf("FAIL\nmalloc: Memory_2 was null after malloc at iteration %lu\n", i);
            free(memory_1);
            return 1;
        }
        if (overlaps(memory_1, memory_2, new_length, new_length)) {
            printf("FAIL\nmalloc: Memory_1 and Memory_2 are overlapping at iteration %lu\n", i);
            free(memory_1);
            free(memory_2);
            return 1;
        }
        for (size_t j = 0; j < new_length; ++j) {
            memory_1[j] = j;
            memory_2[new_length - j - 1] = j;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != j) {
                printf("FAIL\nmalloc: Memory_1[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], j, i);
                return 1;
            }
            if (memory_2[j] != new_length - j - 1) {
                printf("FAIL\nmalloc: Memory_2[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], new_length - j - 1, i);
                return 1;
            }
        }
    }
    printf("OK\n");

    printf("10: calloc and freeing %lu times\ntesting... ", iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        free(memory_1);
        free(memory_2);
        memory_1 = calloc(new_length, sizeof(unsigned long int));
        if (!memory_1) {
            printf("FAIL\ncalloc: Memory_1 was null after calloc at iteration %lu\n", i);
            return 1;
        }
        memory_2 = calloc(new_length, sizeof(unsigned long int));
        if (!memory_2) {
            printf("FAIL\ncalloc: Memory_2 was null after calloc at iteration %lu\n", i);
            free(memory_1);
            return 1;
        }
        if (overlaps(memory_1, memory_2, new_length, new_length)) {
            printf("FAIL\ncalloc: Memory_1 and Memory_2 are overlapping at iteration %lu\n", i);
            free(memory_1);
            free(memory_2);
            return 1;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != 0) {
                printf("FAIL\ncalloc: Memory_1[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], 0lu, i);
                return 1;
            }
            if (memory_2[j] != 0) {
                printf("FAIL\ncalloc: Memory_2[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], 0lu, i);
                return 1;
            }
        }
    }
    printf("OK\n");

    printf("11: realloc, storing, and freeing %lu times\ntesting... ", iterations);
    for (size_t i = 1; i <= iterations; ++i) {
        new_length = 100 * i;
        memory_1 = realloc(memory_1, new_length * sizeof(unsigned long int));
        if (!memory_1) {
            printf("FAIL\nrealloc: Memory_1 was null after realloc at iteration %lu\n", i);
            return 1;
        }
        memory_2 = realloc(memory_2, new_length * sizeof(unsigned long int));
        if (!memory_2) {
            printf("FAIL\nrealloc: Memory_2 was null after realloc at iteration %lu\n", i);
            free(memory_1);
            return 1;
        }
        if (overlaps(memory_1, memory_2, new_length, new_length)) {
            printf("FAIL\nrealloc: Memory_1 and Memory_2 are overlapping at iteration %lu\n", i);
            free(memory_1);
            free(memory_2);
            return 1;
        }
        for (size_t j = 0; j < new_length; ++j) {
            memory_1[j] = j;
            memory_2[new_length - j - 1] = j;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (memory_1[j] != j) {
                printf("FAIL\nrealloc: Memory_1[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_1[j], j, i);
                return 1;
            }
            if (memory_2[j] != new_length - j - 1) {
                printf("FAIL\nrealloc: Memory_2[%lu] == %lu, and not %lu at iteration %lu\n", j, memory_2[j], new_length - j - 1, i);
                return 1;
            }
        }
    }
    printf("OK\n");

    printf("12: freeing NULL\ntesting... ");
    free(NULL);
    printf("OK\n");

    printf("13: freeing memory\ntesting... ");
    free(memory_1);
    free(memory_2);
    printf("OK\n");

    size_t allocations = 10000;
    new_length = 1000;
    printf("14: malloc, store, and free %lu arrays of length %lu randomly\ntesting... ", allocations, new_length);
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

    printf("15: calloc, store, and free %lu arrays of length %lu randomly\ntesting... ", allocations, new_length);
    memories = calloc(allocations, sizeof(unsigned long int*));
    shuffle(memories, allocations);
    for (size_t i = 0; i < allocations; i++) {
        memories[i] = calloc(new_length, sizeof(unsigned long int));
        if (!memories[i]) {
            printf("FAIL\ncalloc: Memory was null after calloc at iteration %lu\n", i);
            return 1;
        }
        for (size_t j = 0; j < new_length; ++j) {
            if (i % 2)
                memories[i][j] = j;
        }
    }
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

    return 0;
}