#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void *worker(void *arg) {
  char *name = (char *)arg;

  for (int i = 0; i < 10; i++) {
    printf("[%s}: working [%d} \n", name, i);
    sleep(1);
  }
  return NULL;
}

int main() {
  pthread_t t1, t2, t3;

  pthread_create(&t1, NULL, worker, "Thread 1");
  pthread_create(&t2, NULL, worker, "Thread 2");
  pthread_create(&t3, NULL, worker, "Thread 3");

  // pthread_join(t1, NULL);
  // pthread_join(t2, NULL);
  // pthread_join(t3, NULL);

  printf("Done! \n");
  return 0;
}
