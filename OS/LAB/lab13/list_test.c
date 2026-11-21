#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  char key;
  struct node *next;
} node;

typedef struct {
  node *head;
  pthread_mutex_t lock;
} list;

void init_list(list *my_list) {
  my_list->head = NULL;
  pthread_mutex_init(&my_list->lock, NULL);
}

void insert_in_list(list *my_list, char key) {
  node *new_node = malloc(sizeof(node));
  new_node->key = key;
  pthread_mutex_lock(&my_list->lock);
  new_node->next = my_list->head;
  my_list->head = new_node;
  pthread_mutex_unlock(&my_list->lock);
}

int find_list_size(list *my_list) {
  int count = 0;
  pthread_mutex_lock(&my_list->lock);
  node *curr = my_list->head;
  while (curr) {
    count++;
    curr = curr->next;
  }
  pthread_mutex_unlock(&my_list->lock);
  return count;
}

void *insert_many(void *arg) {
  list *my_list = (list *)arg; // pass by reference

  for (int i = 0; i < 100000; i++) {
    insert_in_list(my_list, i + 1);
  }
  return NULL;
}

int main() {
  list my_list;
  init_list(&my_list);

  pthread_t threads[4];
  for (int i = 0; i < 4; i++) {
    pthread_create(&threads[i], NULL, insert_many, &my_list);
  }

  for (int i = 0; i < 4; i++) {
    pthread_join(threads[i], NULL);
  }
  int size = find_list_size(&my_list);

  printf("Size of the list: %d\n", size);
  return 0;
}
