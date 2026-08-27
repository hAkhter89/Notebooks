#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Check if at least one argument
    if (argc > 1) {

        int n = atoi(argv[1]);



        // Loop
        for (int i = 0; i < n; i++) {
            printf("%s\n", argv[2]);
        }
    }

    else {
        printf("None \n");
    }

    return 0;
}
