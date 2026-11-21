#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int *data;
  int size;
  pthread_mutex_t lock;
} array;

void init(array *arr, int n) {
  arr->size = n;
  arr->data = (int *)calloc(n, sizeof(int)); // by reference
  pthread_mutex_init(&arr->lock, NULL);
}

void set(array *arr, int k, int value) {
  if (k < 0 || k >= arr->size)
    return;
  pthread_mutex_lock(&arr->lock);
  arr->data[k] = value;
  pthread_mutex_unlock(&arr->lock);
}

int get(array *arr, int k) {
  if (k < 0 || k >= arr->size)
    return -1;
  pthread_mutex_lock(&arr->lock);
  int value = arr->data[k];
  pthread_mutex_unlock(&arr->lock);
  return value;
}

void increment(array *arr, int k) {
  if (k < 0 || k >= arr->size)
    return;
  pthread_mutex_lock(&arr->lock);
  arr->data[k]++;
  pthread_mutex_unlock(&arr->lock);
}

typedef struct {
  array *arr;
  char *filename;
} thread_data;

void *process_file(void *arg) {
  thread_data *td = (thread_data *)arg;
  FILE *file = fopen(td->filename, "r");

  if (file == NULL) {
    printf("Error: Could not open file %s\n", td->filename);
    return NULL;
  }

  int ch;
  while ((ch = fgetc(file)) != EOF) {
    if (isalpha(ch)) {
      char lower = tolower(ch);
      if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' ||
          lower == 'u') {
        increment(td->arr, 0); // Vowels in index 0
      } else {
        increment(td->arr, 1); // Consonants in index 1
      }
    } else if (isdigit(ch)) {
      increment(td->arr, 2); // Digits in index 2
    } else if (!isspace(ch)) {
      increment(td->arr, 3); // Special characters in index 3
    }
  }

  fclose(file);
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    printf("Incorrect format: %s <file1> <file2> <file3> <file4> <file5>\n",
           argv[0]);
    return 1;
  }

  array character_array;
  init(&character_array, 4); // Need 4 indexes for the counts (0 to 3)

  pthread_t threads[5];
  thread_data t_data[5];

  // Assign files to threads
  for (int i = 0; i < 5; i++) {
    t_data[i].arr = &character_array;
    t_data[i].filename = argv[i + 1];
    pthread_create(&threads[i], NULL, process_file, &t_data[i]);
  }

  for (int i = 0; i < 5; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("\n");
  printf("Vowels (Index 0): %d\n", get(&character_array, 0));
  printf("Consonants (Index 1): %d\n", get(&character_array, 1));
  printf("Digits (Index 2): %d\n", get(&character_array, 2));
  printf("Special Characters (Index 3): %d\n", get(&character_array, 3));

  free(character_array.data);
  pthread_mutex_destroy(&character_array.lock);

  return 0;
}
