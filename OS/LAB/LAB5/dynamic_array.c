#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int n;

    printf("How many integers do you want to store? ");
    fflush(stdout);
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Error\n");
        return 1;
    }
    if (n <= 0) {
        fprintf(stderr, "must be positive (got %d).\n", n);
        return 1;
    }
    int *arr = malloc((size_t) n * sizeof(int));

    /* (c) + (d) */
    if (arr == NULL) {
        fprintf(stderr, "failed\n");
        return 1;
    }

    printf("Allocated %zu bytes at %p\n", (size_t) n * sizeof(int), (void *) arr);

    for (int i = 0; i < n; i++) {
        printf("  arr[%d] = ", i);
        fflush(stdout);
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Error: bad input at index %d.\n", i);
            free(arr);                 /* clean up */
            return 1;
        }
    }

    /* print */
    printf("Array contents:");
    for (int i = 0; i < n; i++)
        printf(" %d", arr[i]);
    printf("\n");

    free(arr);
    arr = NULL;

    return 0;
}
