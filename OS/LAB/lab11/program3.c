#include <pthread.h>
#include <stdio.h>

int result1, result2;

void *square(void *arg) {
  int num = *(int *)arg;
  result1 = num * num;
  return NULL;
}

void *cube(void *arg) {
  int num = *(int *)arg;
  result2 = num * num * num;
  return NULL;
}

int main() {
  pthread_t t1, t2;
  int num = 3;
  pthread_create(&t1, NULL, square, &num);
  pthread_create(&t2, NULL, cube, &num);

  //    pthread_join(t1, NULL);
  // pthread_join(t2, NULL);
  printf("Square: [%d], Cube: [%d] \n", result1, result2);
  return 0;
}
