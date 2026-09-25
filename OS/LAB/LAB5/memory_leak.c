#include <stdio.h>
#include <stdlib.h>

static void leak_inside_a_function(void) {
  int *block = malloc(50 * sizeof(int)); /* 200 bytes */
  if (block == NULL)
    return;
  block[0] = 1;
  // when the fucntion returns, block dissapears but the data is still
  // allocated, since there is no pointer to it anymore we say it is lost or
  // leaked free(block) would fix the leak
}

int main(void) {

  int *data = malloc(100 * sizeof(int)); /* 400 bytes */
  if (data == NULL) {
    fprintf(stderr, "Memory allocation failed!\n");
    return 1;
  }

  for (int i = 0; i < 10; i++)
    data[i] = i * 100;

  printf("Memory allocated at %p and used some of it.\n", (void *)data);

  leak_inside_a_function();

  // free(DATA)
  printf("Program finished, memory was not deallocated.\n");
  return 0;
}
