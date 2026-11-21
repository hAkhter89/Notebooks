#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  int id, work_count, delay;
  char *name;
} ThreadData;

void *worker(void *arg) {
  ThreadData *data = (ThreadData *)arg;
  int i, limit = data->work_count, delay = data->delay;
  pthread_t thread_id = pthread_self();

  for (i = 0; i < limit; i++) {
    printf("[%lu] Worker [%s]: task [%d] \n", thread_id, data->name, i);
    sleep(data->delay);
  }
  return NULL;
}

int main() {
  pthread_t t1, t2, t3;

  ThreadData data1 = {1, 5, 2, "sun"};
  ThreadData data2 = {2, 2, 3, "moon"};
  ThreadData data3 = {3, 3, 4, "mars"};

  pthread_create(&t1, NULL, worker, &data1);
  pthread_create(&t2, NULL, worker, &data2);
  pthread_create(&t3, NULL, worker, &data3);
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  return 0;
}
