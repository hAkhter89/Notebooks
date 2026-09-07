#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // Needed for wait()

int main(int argc, char *argv[]) {
    // FORK1
    int return_child = fork();

    if (return_child < 0) {
        fprintf(stderr, "fork 1 failed\n");
        exit(1);
    }
    else if (return_child == 0) {
        // Child 1
        printf("hello, I am child 1 (pid:%d)\n", (int) getpid());
    }
    else {
        // wait for child1
        int wc = wait(NULL);
        int proc_id = (int) getpid();
        printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", return_child, wc, proc_id);

        // fork2
        int return_child2 = fork();

        if (return_child2 < 0) {
            fprintf(stderr, "fork 2 failed\n");
            exit(1);
        }
        else if (return_child2 == 0) {
            printf("hello, I am child 2 (pid:%d)\n", (int) getpid());
        }
        else {
            // wait for child2
            int wc2 = wait(NULL);
            printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", return_child2, wc2, proc_id);

            // fork3
            int return_child3 = fork();

            if (return_child3 < 0) {
                fprintf(stderr, "fork 3 failed\n");
                exit(1);
            }
            else if (return_child3 == 0) {
                printf("hello, I am child 3 (pid:%d)\n", (int) getpid());
            }
            else {
                // wait for child3
                int wc3 = wait(NULL);
                printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", return_child3, wc3, proc_id);
            }
        }
    }

    return 0;
}
