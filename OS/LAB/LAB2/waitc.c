#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // Needed for wait()

int main(int argc, char *argv[]) {
    int proc_id = (int) getpid();
    // FORK1
    pid_t return_child = fork();

    if (return_child < 0) {
        fprintf(stderr, "fork 1 failed\n");
        exit(1);
    }
    else if (return_child == 0) {
        // Child 1
        printf("hello, I am child 1 (pid:%d)\n", (int) getpid());
    }
    else {
        // fork2
        pid_t return_child2 = fork();

        if (return_child2 < 0) {
            fprintf(stderr, "fork 2 failed\n");
            exit(1);
        }
        else if (return_child2 == 0) {
            printf("hello, I am child 2 (pid:%d)\n", (int) getpid());
        }
        else {
            // fork3
            pid_t return_child3 = fork();

            if (return_child3 < 0) {
                fprintf(stderr, "fork 3 failed\n");
                exit(1);
            }
            else if (return_child3 == 0) {
                printf("hello, I am child 3 (pid:%d)\n", (int) getpid());
            }
            else {
                // wait block
                waitpid(return_child, NULL, 0);
                printf("Parent of child 1 (wc:%d) (pid:%d)\n", return_child, proc_id);
                waitpid(return_child2, NULL, 0);
                printf("Parent of child 2 (wc:%d) (pid:%d)\n", return_child2, proc_id);
                waitpid(return_child3, NULL, 0);
                printf("Parent of child 3 (wc:%d) (pid:%d)\n", return_child3, proc_id);

            }
        }
    }

    return 0;
}
