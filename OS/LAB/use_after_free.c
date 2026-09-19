/* use_after_free.c  --  Task 5(b)
 *
 * Expected valgrind report:
 *   Invalid write of size 4   ... Address 0x... is 0 bytes inside a block of
 *   size 16 free'd            ... (plus the stack trace of the free)
 *
 *   gcc -Wall -Wextra -g -O0 use_after_free.c -o use_after_free
 *   valgrind ./use_after_free
 */

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(4 * sizeof(int));
    if (p == NULL) return 1;

    p[0] = 1234;
    printf("before free: p[0] = %d   (at %p)\n", p[0], (void *) p);

    free(p);                 /* the block goes back to the allocator */

    p[0] = 5678;             /* INVALID WRITE - p is now a dangling pointer */
    printf("after  free: p[0] = %d\n", p[0]);   /* INVALID READ */

    /* Usually no crash: the page is still mapped, so the CPU sees nothing
     * wrong. We are quietly scribbling on the allocator's bookkeeping.    */
    return 0;
}
