#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])  {
  if (argc != 6 ){
    printf("incorrect input\n");
    return 1;
  }

  int a = atoi(argv[1]);
  int num;
  int sum = a;
  int min = a;
  int max = a;

  for (int i = 2; i <= 5; i++) {
    num = atoi(argv[i]);
    sum += num;
    if (num < min) {
      min = num;
    }
    if (num > max) {
      max = num;
    }
  }
  printf("Sum %d\n", sum);
  printf("Min %d\n", min);
  printf("Sum %d\n", max);



  return 0;

}
