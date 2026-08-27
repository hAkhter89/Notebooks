#include <stdio.h>
#include <stdlib.h> // For qsort
#include <string.h> // For strcmp


int compare(const void *a, const void *b) {
    // Because argv is an array of pointers, we need to cast to a pointer-to-a-pointer (char **)
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("N/A\n");
        return 1;
    }

    // Starts at argv[1]
    qsort(&argv[1], argc - 1, sizeof(char *), compare);

    // Print the sorted list
    printf("Sorted words:\n");
    for (int i = 1; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    return 0;
}
