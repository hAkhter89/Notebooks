#include <pthread.h>
#include <stdio.h>

int balance = 0; // Shared bank account

void *deposit(void *arg) {
  for (int i = 0; i < 100000; i++) {
    balance = balance + 10; // Unsafe: race condition!
  }
  return NULL;
}

int main() {
  pthread_t t1, t2;

  printf("Starting balance: %d\n", balance);

  pthread_create(&t1, NULL, deposit, NULL);
  pthread_create(&t2, NULL, deposit, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  printf("Final balance: %d\n", balance);
  printf("Expected: 2000000\n");

  if (balance != 2000000) {
    printf("Race! Missing: %d\n", 2000000 - balance);
  }

  return 0;
}
