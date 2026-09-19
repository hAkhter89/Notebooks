
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *p = malloc(sizeof(int));
    if (p == NULL) return 1;

    *p = 7;
    printf("allocated %p, value %d\n", (void *) p, *p);

    free(p);
    printf("freed once.\n");

    free(p);                 /* DOUBLE FREE */
    printf("freed twice\n");

    return 0;
}
