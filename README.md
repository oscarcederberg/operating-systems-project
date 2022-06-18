# EDAF35 Operating Systems - Heap Managers
Oscar Cederberg

## Assignment
1. Your task is to implement two versions of malloc/calloc/realloc and free: One should be a linked-list implementation and the other an implementation of the buddy system.
2. You need to use `sbrk` to allocate memory from the kernel on Linux for the list version and increment the heap size in suitable steps. For the buddy system, you are allowed to allocate a sufficiently large block at once.
3. At a free, you must merge adjacent unallocated blocks in the list version and buddies in the buddy version.
4. Your implementations should work with gawk. Use any version (versions 3.x are recommended, being known to work properly; versions >= 4 may cause problems).
5. You may work on your project in pairs (as during the lab, or other pairs), but you will be asked questions individually - both group members should be able to answer any question.

## Steps
1. Download the `gawk` sources (a version from e.g. https://ftp.gnu.org/gnu/gawk/) and run `configure` and then `make check`.
2. Then edit the `Makefile` to use your malloc (and the rest) implementation. To edit the `Makefile`, look for places where `replace` is used and add your file in a similar way.
3. …Then run `make check` again and improve until you succeed.

Repeat the same procedure for your other malloc implementation.
