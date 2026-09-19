
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int local_stack = 0;                           // stack
    void *heap   = malloc(2 * 1024 * 1024);        // heap

    if (heap == NULL) {
        fprintf(stderr, "Failed to allocate heap memory\n");
        return 1;
    }
    printf("main address: %p\n", (void*)main);
    printf("Heap address: %p\n", heap);
    printf("Stack address: %p\n", &local_stack);
    return 0;
}
