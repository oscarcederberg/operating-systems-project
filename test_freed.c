#include <stdio.h>
#include <limits.h>

#include "malloc.c"

int main() {
    printf("running tests...\n");

    size_t num = 16;
    size_t size;
    for (size_t order = 0; order < 4; order++) {
        size = num * (1UL << order) * sizeof(unsigned long int);
        printf("malloc size %lu\ntesting....\n", size);
        unsigned long int* memory_1 = malloc(size);
        unsigned long int* memory_2 = malloc(size);

        if (!memory_1) {
                printf("memory_1 was not allocated successfully\n");
                return 1;
        }
        if (!memory_1) {
                printf("memory_1 was not allocated successfully\n");
                return 2;
        }

        for (size_t i = 0; i < 16; i++) {
            memory_1[i] = i;
            memory_2[i] = i;
        }

        for (size_t i = 0; i < 16; i++) {
            if (memory_1[i] != i) {
                printf("memory_1[%lu] was %lu, expected %lu\n", i, memory_1[i], i);
            }
            if (memory_2[i] != i) {
                printf("memory_2[%lu] was %lu, expected %lu\n", i, memory_2[i], i);
            }
        }

        printf("freeing memory_1\n");
        free(memory_1);

        for (size_t i = 0; i < get_num_orders(); i++) {
            if (i < order && get_num_free_blocks(i) != 0) {
                printf("there are %lu free blocks of order %lu, expected 0\n", get_num_free_blocks(i), i);
            }
            if (i >= order && get_num_free_blocks(i) != 1) {
                printf("there are %lu free blocks of order %lu, expected 1\n", get_num_free_blocks(i), i);
            }
        }
        
        if (get_num_free_blocks(get_num_orders()) != 0) {
            printf("there are %lu free blocks of order %lu, expected 0\n", get_num_free_blocks(order), get_num_orders());
        }

        printf("freeing memory_2\n");
        free(memory_2);

        for (size_t i = 0; i < get_num_orders(); i++) {
            if (get_num_free_blocks(i) != 0) {
                printf("there are %lu free blocks of order %lu, expected 0\n", get_num_free_blocks(i), i);
            }
        }
        
        if (get_num_free_blocks(get_num_orders()) != 1) {
            printf("there are %lu free blocks of order %lu, expected 1\n", get_num_free_blocks(order), get_num_orders());
        }
    }
}