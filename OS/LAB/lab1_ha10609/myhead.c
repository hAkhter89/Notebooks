#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *filename = argv[1];
  int n = atoi(argv[2]);

  FILE *file = fopen(filename, "r");
  char line[100];

  if (file == NULL)
  {
    printf("no file here\n");
    return 1;
  }
  int count  = 0;

  while (count < n && fgets(line, sizeof(line), file)) {
    printf("%s", line);
    count++;
  }
  return 0;
}
