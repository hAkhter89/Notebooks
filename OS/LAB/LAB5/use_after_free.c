#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(4 * sizeof(int));
    if (p == NULL) return 1;

    p[0] = 1234;
    printf("before free: p[0] = %d   (at %p)\n", p[0], (void *) p);

    free(p);                 /* block goes back to the allocator */

    p[0] = 5678;             /* INVALID WRITE - p is now a dangling pointer */
    printf("after  free: p[0] = %d\n", p[0]);   /* INVALID READ */
    return 0;
}
