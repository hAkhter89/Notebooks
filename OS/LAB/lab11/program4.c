#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int input, output;
} CalcData;

void *calculator(void *arg) {
  CalcData *data = (CalcData *)arg; // type cast the void*

  int factorial = 1;
  for (int i = 1; i <= data->input; i++) {
    factorial *= i;
  }

  data->output = factorial; // Store result in struct

  return NULL;
}

int main() {
  pthread_t thread;

  CalcData *data = (CalcData *)malloc(sizeof(CalcData));

  data->input = 5;
  data->output = 0; // input=5, output=0

  pthread_create(&thread, NULL, calculator, data);
  pthread_join(thread, NULL);

  printf("Result: [%d] \n", data->output); // Now contains 120

  free(data);

  return 0;
}
