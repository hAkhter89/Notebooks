#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALLOC_SIZE 4096

int main(void) {
    void *addr = mmap(NULL, ALLOC_SIZE, PROT_READ | PROT_WRITE,
                       MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap failed");
        return EXIT_FAILURE;
    }

    printf("Allocated %d bytes at address: %p\n", ALLOC_SIZE, addr);

    int *int_array = (int *)addr;
    for (int i = 0; i < ALLOC_SIZE / (int)sizeof(int); i++)
        int_array[i] = i * 100;

    printf("Wrote data to the memory region.\n");

    printf("Sample data from memory: %d, %d\n", int_array[6], int_array[7]);

    if (munmap(addr, ALLOC_SIZE) == -1) {
        perror("munmap failed");
        return EXIT_FAILURE;
    }

    printf("Successfully deallocated memory at address: %p\n", addr);

    return 0;
}