#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string> <file>\n", argv[0]);
        exit(1);
    }

    int proc_id = (int) getpid();

    pid_t return_child = fork();

    if (return_child < 0) {
        fprintf(stderr, "fork 1 failed\n");
        exit(1);
    }
    else if (return_child == 0) {
        printf("hello, I am child 1 (pid:%d)\n", (int) getpid());

        char *myargs[4];
        myargs[0] = strdup("grep");
        myargs[1] = strdup("print");
        myargs[2] = strdup(argv[2]);
        myargs[3] = NULL;

        execvp(myargs[0], myargs);
        fprintf(stderr, "exec failed\n");
        exit(1);
    }
    else {
        pid_t return_child2 = fork();

        if (return_child2 < 0) {
            fprintf(stderr, "fork 2 failed\n");
            exit(1);
        }
        else if (return_child2 == 0) {
            printf("hello, I am child 2 (pid:%d)\n", (int) getpid());

            char *myargs[3];
            myargs[0] = strdup("wc");
            myargs[1] = strdup(argv[2]);
            myargs[2] = NULL;

            execvp(myargs[0], myargs);
            fprintf(stderr, "exec failed child2\n");
            exit(1);
        }
        else {
            pid_t return_child3 = fork();

            if (return_child3 < 0) {
                fprintf(stderr, "fork 3 failed\n");
                exit(1);
            }
            else if (return_child3 == 0) {
                printf("hello, I am child 3 (pid:%d)\n", (int) getpid());

                char *myargs[3];
                myargs[0] = strdup("cat");
                myargs[1] = strdup(argv[2]);
                myargs[2] = NULL;

                execvp(myargs[0], myargs);
                fprintf(stderr, "exec failed child3\n");
                exit(1);
            }
            else {
                pid_t return_child4 = fork();

                if (return_child4 < 0) {
                    fprintf(stderr, "fork 4 failed\n");
                    exit(1);
                }
                else if (return_child4 == 0) {
                    printf("hello, I am child 4 (pid:%d)\n", (int) getpid());

                    char *myargs[3];
                    myargs[0] = strdup("echo");
                    myargs[1] = strdup(argv[1]);
                    myargs[2] = NULL;

                    execvp(myargs[0], myargs);
                    fprintf(stderr, "exec failed child4\n");
                    exit(1);
                }
                else {
                    waitpid(return_child, NULL, 0);
                    printf("Parent of child 1 (wc:%d) (pid:%d)\n", return_child, proc_id);

                    waitpid(return_child2, NULL, 0);
                    printf("Parent of child 2 (wc:%d) (pid:%d)\n", return_child2, proc_id);

                    waitpid(return_child3, NULL, 0);
                    printf("Parent of child 3 (wc:%d) (pid:%d)\n", return_child3, proc_id);

                    waitpid(return_child4, NULL, 0);
                    printf("Parent of child 4 (wc:%d) (pid:%d)\n", return_child4, proc_id);
                }
            }
        }
    }

    return 0;
}
