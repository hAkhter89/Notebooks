#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char key;
  struct node *next;
} node;

typedef struct {
  node *head;
  node *tail;
  int size;
  pthread_mutex_t lock;
} queue;
void init_queue(queue *q) {
  q->head = NULL;
  q->tail = NULL;
  q->size = 0;
  pthread_mutex_init(&q->lock, NULL);
}

int is_empty(queue *q) {
  pthread_mutex_lock(&q->lock);
  int empty = (q->size == 0);
  pthread_mutex_unlock(&q->lock);
  return empty;
}

int is_full(queue *q) { return 0; }

void enqueue(queue *q, char key) {
  node *new_node = malloc(sizeof(node));
  new_node->key = key;
  new_node->next = NULL;

  pthread_mutex_lock(&q->lock);
  if (q->tail == NULL) {
    q->head = new_node;
    q->tail = new_node;
  } else {
    q->tail->next = new_node;
    q->tail = new_node;
  }
  q->size++;
  pthread_mutex_unlock(&q->lock);
}

char dequeue(queue *q) {
  pthread_mutex_lock(&q->lock);
  if (q->head == NULL) {
    pthread_mutex_unlock(&q->lock);
    return '\0'; // Queue is empty
  }

  node *temp = q->head;
  char key = temp->key;

  q->head = q->head->next;
  if (q->head == NULL) {
    q->tail = NULL; // If last element was removed, tail becomes NULL
  }
  q->size--;
  pthread_mutex_unlock(&q->lock);

  free(temp);
  return key;
}

void *insert_many(void *arg) {
  queue *q = (queue *)arg;
  for (int i = 0; i < 100000; i++) {
    enqueue(q, i);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("invalid\n");
    return 1;
  }

  int num_threads = atoi(argv[1]);
  if (num_threads <= 0) {
    printf("invalid\n");
    return 1;
  }

  queue my_queue;
  init_queue(&my_queue);

  // based on command line argument
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));

  for (int i = 0; i < num_threads; i++) {
    pthread_create(&threads[i], NULL, insert_many, &my_queue);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  int expected_size = num_threads * 100000;

  printf("threads: %d\n", num_threads);
  printf("Expected: %d\n", expected_size);
  printf("Actual: %d\n", my_queue.size);

  free(threads);
  return 0;
}
