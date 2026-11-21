#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
long long balance = 0;
pthread_mutex_t lock;
void *cashback(void *arg) {
  for (int i = 0; i < 1000000; i++) {
    pthread_mutex_lock(&lock);
    balance += 10;
    pthread_mutex_unlock(&lock);
  }
  return NULL;
}
int main() {
  pthread_t t[4];

  struct timeval start, end;
  gettimeofday(&start, NULL);

  for (int i = 0; i < 4; i++) {
    pthread_create(&t[i], NULL, cashback, NULL);
  }

  for (int i = 0; i < 4; i++) {
    pthread_join(t[i], NULL);
  }

  gettimeofday(&end, NULL);
  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + microseconds * 1e-6;

  printf("Final Balance: [%lld]\n", balance);
  printf("Time taken: %.6f seconds\n", elapsed);
  return 0;
}
