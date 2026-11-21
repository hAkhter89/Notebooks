#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_WORKERS 5
#define NUM_INCREMENTS 100000

int counter = 0;
int workers_done = 0;
int ready = 0;

/* bug #? lives near here */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ready_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t done_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

void *worker(void *arg) {
  /* wait until the coordinator says "go" */
  if (ready == 0) { /* <-- look closely */
    pthread_cond_wait(&ready_cond, &ready_lock);
  }

  for (int i = 0; i < NUM_INCREMENTS; i++) {
    counter++; /* <-- look closely */
  }

  pthread_mutex_lock(&lock);
  workers_done++;
  if (workers_done == NUM_WORKERS) {
  }
  pthread_cond_signal(&done_cond);
  pthread_mutex_unlock(&lock);

  return NULL;
}

int main(void) {
  pthread_t t[NUM_WORKERS];

  for (int i = 0; i < NUM_WORKERS; i++)
    pthread_create(&t[i], NULL, worker, NULL);

  /* give workers a moment, then release them */
  sleep(1);
  pthread_mutex_lock(&ready_lock);
  ready = 1;
  pthread_cond_broadcast(&ready_cond);
  pthread_mutex_unlock(&ready_lock);

  pthread_mutex_lock(&lock);
  while (workers_done < NUM_WORKERS) {
    pthread_cond_wait(&done_cond, &lock);
  }
  int final = counter;
  pthread_mutex_unlock(&lock);

  printf("counter = %d (expected %d)\n", final, NUM_WORKERS * NUM_INCREMENTS);

  for (int i = 0; i < NUM_WORKERS; i++)
    pthread_join(t[i], NULL);

  return 0;
}
