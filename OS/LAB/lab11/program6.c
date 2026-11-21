#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int cancer_cell_count = 0;

void *cancer_cell(void *arg) {
  int id = *(int *)arg;
  printf("Cancel Cell: Cell %d activated. \n", id);

  // Malicious growth behavior
  while (1) {
    for (int i = 0; i < 1000000; i++)
      ; // Consume system resources

    if (cancer_cell_count < 10) // Uncontrolled replication
    {
      pthread_t new_cell;
      int new_id = cancer_cell_count++;
      pthread_create(&new_cell, NULL, cancer_cell, &new_id);
      printf("Metastasis: Cell %d spawned cell %d\n", id, new_id);
    }
  }
  return NULL;
}

int main() {
  printf("Cancer Simulation: Cancer cell growth pattern. \n");
  printf("Demonstrating uncontrolled cellular replication.\n\n");

  pthread_t initial_cell;
  int id1 = 0;
  cancer_cell_count = 1;

  // Initial malignant cell
  pthread_create(&initial_cell, NULL, cancer_cell, &id1);

  sleep(3); // Let the cancer spread for 3 seconds

  printf("\n Body collapsing \n");
  printf("[%d] Cancer cells eating resources!\n", cancer_cell_count);
  printf("Healthy system practices prevent this:\n");
  printf("- Controlled cell growth mechanisms\n");
  printf("- Apoptosis (programmed cell death)\n");
  printf("- Immune system regulation\n");

  return 0;
}
