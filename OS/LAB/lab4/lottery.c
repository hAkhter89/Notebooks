#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 1. Structure of a Lottery Process
typedef struct {
  int pid;
  int burst_time;
  int remaining_time;
  int tickets;
} Process;

// Sample Initialization: Note how PID 1 holds the vast majority of tickets
Process sample_lottery_jobs[] = {
    {1, 5, 5, 60}, // PID 1: 5ms burst, 60 tickets (High Priority)
    {2, 5, 5, 30}, // PID 2: 5ms burst, 30 tickets (Medium Priority)
    {3, 5, 5, 10}  // PID 3: 5ms burst, 10 tickets (Low Priority)
};

#define NUM_JOBS 3

// Tally array: win_count[pid-1] counts how many time slices that pid won
int win_count[NUM_JOBS] = {0};

// 2. Scheduler Logic
void run_lottery(Process jobs[], int num_jobs) {
  int total_tickets = 0;
  for (int i = 0; i < num_jobs; i++)
    total_tickets += jobs[i].tickets;

  int completed = 0;

  while (completed < num_jobs) {
    int winning_ticket = rand() % total_tickets;
    int ticket_count = 0;

    for (int i = 0; i < num_jobs; i++) {
      if (jobs[i].remaining_time > 0) {
        ticket_count += jobs[i].tickets;
        if (winning_ticket < ticket_count) {
          printf("Winning Ticket: %d | Process %d runs!\n", winning_ticket,
                 jobs[i].pid);
          jobs[i].remaining_time--;
          win_count[jobs[i].pid - 1]++; // tally this time slice

          if (jobs[i].remaining_time == 0) {
            completed++;
            total_tickets -= jobs[i].tickets;
          }
          break;
        }
      }
    }
  }
}

int main(void) {
  srand(time(NULL));

  int total = 0;
  const int NUM_RUNS = 5;

  for (int run = 1; run <= NUM_RUNS; run++) {
    // Reset the jobs for a fresh run each time
    Process jobs[NUM_JOBS] = {{1, 5, 5, 60}, {2, 5, 5, 30}, {3, 5, 5, 10}};

    printf("=================================\n");
    printf("           CASINO RUN %d\n", run);
    printf("=================================\n");

    run_lottery(jobs, NUM_JOBS);

    int slices_this_run = 0;
    for (int i = 0; i < NUM_JOBS; i++)
      slices_this_run += jobs[i].burst_time;
    total += slices_this_run;

    printf("\n");
  }

  // Empirical tally across all 5 runs
  printf(" EMPIRICAL RESULTS AFTER %d RUNS (%d total slices)\n", NUM_RUNS,
         total);
  printf("PID | Tickets | Wins | Empirical %% | Theoretical %%\n");

  int total_tickets = 0;
  for (int i = 0; i < NUM_JOBS; i++)
    total_tickets += sample_lottery_jobs[i].tickets;

  for (int i = 0; i < NUM_JOBS; i++) {
    double empirical_pct = 100.0 * win_count[i] / total;
    double theoretical_pct =
        100.0 * sample_lottery_jobs[i].tickets / total_tickets;
    printf(" %d  |   %-5d |  %-3d | %10.2f%% | %10.2f%%\n",
           sample_lottery_jobs[i].pid, sample_lottery_jobs[i].tickets,
           win_count[i], empirical_pct, theoretical_pct);
  }

  return 0;
}
