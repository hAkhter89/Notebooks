#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void *child(void *arg) {
  printf("child\n");
  pthread_mutex_lock(&m);
  done = 1;
  pthread_cond_signal(&c);
  pthread_mutex_unlock(&m);
  return NULL;
}

int main(int argc, char *argv[]) {
  pthread_t p;
  printf("parent: begin\n");
  pthread_create(&p, NULL, child, NULL);
  sleep(1);

  pthread_mutex_lock(&m);
  while (done == 0) {
    pthread_cond_wait(&c, &m);
  }
  pthread_mutex_unlock(&m);

  printf("parent: end\n");
  return 0;
}
