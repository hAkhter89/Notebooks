#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h> // For strdup()

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pattern> <file>\n", argv[0]);
        exit(1);
    }

    printf("Hello world (pid:%d)\n", (int) getpid());

    int rc = fork();

    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (rc == 0) {
        printf("hello, I am child (pid:%d)\n", (int) getpid());

        char *myargs[4];
        myargs[0] = strdup("grep");
        myargs[1] = strdup(argv[1]);  // pattern to search
        myargs[2] = strdup(argv[2]);  // file
        myargs[3] = NULL;             //

        execvp(myargs[0], myargs);    // grep <pattern> <file>


        fprintf(stderr, "exec failed\n");
        exit(1);
    }
    else {
        int wc = wait(NULL);
        int proc_id = (int) getpid();
        printf("hello, I am parent of %d (wc:%d) (pid:%d)\n", rc, wc, proc_id);
    }

    return 0;
}
