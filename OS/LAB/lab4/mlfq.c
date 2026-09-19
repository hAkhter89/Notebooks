#include <stdio.h>

#define MAX_JOBS 10
#define Q_LEVELS 3
#define BOOST_TIME 15
#define NUM_JOBS 3

typedef struct {
  int id;
  int burst_time;
  int remaining_time;
  int current_queue;
  int first_run;
  int end_time;
  int allot_left;
} Job;

int queue[Q_LEVELS][MAX_JOBS];
int q_len[Q_LEVELS];

void enqueue(int q, int idx) { queue[q][q_len[q]++] = idx; }

int dequeue(int q) {
  if (q_len[q] == 0)
    return -1;
  int idx = queue[q][0];
  for (int i = 1; i < q_len[q]; i++)
    queue[q][i - 1] = queue[q][i];
  q_len[q]--;
  return idx;
}

void run_mlfq(Job jobs[], int quanta[], const char *label) {
  int time = 0, completed = 0, running = -1, slice_used = 0, q3_switches = 0;

  for (int i = 0; i < Q_LEVELS; i++)
    q_len[i] = 0;

  for (int i = 0; i < NUM_JOBS; i++) {
    jobs[i].current_queue = 0;
    jobs[i].allot_left = quanta[0];
    jobs[i].first_run = -1;
    enqueue(0, i);
  }

  printf("\n=== %s ===\n", label);
  printf("Run | Q1 | Q2 | Q3\n");

  while (completed < NUM_JOBS) {

    if (BOOST_TIME > 0 && time > 0 && time % BOOST_TIME == 0) {
      if (running != -1) {
        jobs[running].current_queue = 0;
        jobs[running].allot_left = quanta[0];
        enqueue(0, running);
        running = -1;
      }
      for (int q = 1; q < Q_LEVELS; q++) {
        while (q_len[q] > 0) {
          int idx = dequeue(q);
          jobs[idx].current_queue = 0;
          jobs[idx].allot_left = quanta[0];
          enqueue(0, idx);
        }
      }
      printf("---- PRIORITY BOOST ----\n");
    }

    if (running != -1) {
      int active_q = jobs[running].current_queue;
      for (int q = 0; q < active_q; q++) {
        if (q_len[q] > 0) {
          enqueue(active_q, running);
          running = -1;
          break;
        }
      }
    }

    if (running == -1) {
      for (int q = 0; q < Q_LEVELS; q++) {
        if (q_len[q] > 0) {
          running = dequeue(q);
          slice_used = 0;
          if (jobs[running].id == 3 && q == Q_LEVELS - 1)
            q3_switches++;
          break;
        }
      }
    }

    if (running != -1) {
      if (jobs[running].first_run == -1)
        jobs[running].first_run = time;

      int q = jobs[running].current_queue;
      int id = jobs[running].id;
      printf("%-4d| ", time + 1);
      if (q == 0)
        printf("%-2d |    |\n", id);
      else if (q == 1)
        printf("   | %-2d |\n", id);
      else
        printf("   |    | %d\n", id);

      jobs[running].remaining_time--;
      jobs[running].allot_left--;
      slice_used++;

      if (jobs[running].remaining_time == 0) {
        jobs[running].end_time = time + 1;
        completed++;
        running = -1;
      } else if (jobs[running].allot_left <= 0) {
        int next_q = q + 1 < Q_LEVELS ? q + 1 : Q_LEVELS - 1;
        jobs[running].current_queue = next_q;
        jobs[running].allot_left = quanta[next_q];
        enqueue(next_q, running);
        running = -1;
      } else if (slice_used >= 1) {
        enqueue(q, running);
        running = -1;
      }
    }
    time++;
  }

  printf("\nJob | Turnaround | Response\n");
  for (int i = 0; i < NUM_JOBS; i++) {
    printf(" %d  | %10d | %8d\n", jobs[i].id, jobs[i].end_time,
           jobs[i].first_run);
  }
  printf("Job 3 switches into Q3: %d\n", q3_switches);
}

int main(void) {
  Job mlfq1[NUM_JOBS] = {{1, 4, 4}, {2, 5, 5}, {3, 30, 30}};
  int quanta1[Q_LEVELS] = {3, 3, 3};
  run_mlfq(mlfq1, quanta1, "MLFQ1: Strict Quanta (3,3,3)");

  Job mlfq2[NUM_JOBS] = {{1, 4, 4}, {2, 5, 5}, {3, 30, 30}};
  int quanta2[Q_LEVELS] = {3, 6, 12};
  run_mlfq(mlfq2, quanta2, "MLFQ2: Variable Quanta (3,6,12)");

  return 0;
}
