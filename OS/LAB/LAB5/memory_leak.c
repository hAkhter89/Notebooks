

#include <stdio.h>
#include <stdlib.h>

static void leak_inside_a_function(void)
{
    int *block = malloc(50 * sizeof(int));   /* 200 bytes */
    if (block == NULL) return;
    block[0] = 1;
    /* The pointer dies when the function returns. The memory does not.
     * Nothing in the program can ever reach it again => definitely lost.  */
}

int main(void)
{
    printf("Attempting to allocate memory...\n");

    int *data = malloc(100 * sizeof(int));   /* 400 bytes */
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < 10; i++)
        data[i] = i * 100;

    printf("Memory allocated at %p and used some of it.\n", (void *) data);

    leak_inside_a_function();

    /* no free() here. */
    printf("Program, finished, memory was not deallocated.\n");
    return 0;
}
