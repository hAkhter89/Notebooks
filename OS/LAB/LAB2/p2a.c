#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int a = (int) getpid();
  printf("[pid:%d] - Hello Students \n", a);

  int return_child = fork();

  if (return_child < 0) {
    fprintf(stderr, "fork failed\n");
    exit(1);
  }

  if (return_child == 0) {
    printf("[pid:%d] I am the child. My return process = [%d]\n", (int)getpid(), return_child);
  }
  else {
    printf("[pid:%d] I am parent of [%d] \n", (int) getpid(), return_child);
  }
  return 0;

}
