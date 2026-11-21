#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 10
#define NUM_INCREMENTS 100000

int counter = 0;
int threads_done = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done_cond = PTHREAD_COND_INITIALIZER;

void *increment_counter(void *arg) {
  for (int i = 0; i < NUM_INCREMENTS; i++) {
    pthread_mutex_lock(&lock);
    counter++;
    pthread_mutex_unlock(&lock);
  }

  pthread_mutex_lock(&lock);
  threads_done++;
  if (threads_done == NUM_THREADS) {
    pthread_cond_signal(&done_cond); // Notify main thread
  }
  pthread_mutex_unlock(&lock);

  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], NULL, increment_counter, NULL);
  }

  pthread_mutex_lock(&lock);
  while (threads_done < NUM_THREADS) {
    pthread_cond_wait(&done_cond, &lock);
  }
  pthread_mutex_unlock(&lock);

  printf("Final counter: %d (Expected: %d)\n", counter,
         NUM_THREADS * NUM_INCREMENTS);
  return 0;
}
