#include <stdio.h>

struct job
{
char name;
int arrival; // When job arrives
int length; // Original length
int remaining; // Time left to finish
int start_time; // First time job runs (-1 if not started)
int completion_time; // Time job finishes
int finished; // 0 if not done, 1 if done
};

void simulate_stcf(struct job jobs[], int job_count)
{
    int time = 0, done = 0;

    while (done < job_count) {
        int shortest = -1;

        for (int i = 0; i < job_count; i++) {
            if (!jobs[i].finished && jobs[i].arrival <= time &&
                (shortest == -1 ||
                 jobs[i].remaining < jobs[shortest].remaining))
                shortest = i;
        }

        if (shortest == -1) {
            time++;
            continue;
        }

        if (jobs[shortest].start_time == -1)
            jobs[shortest].start_time = time;

        jobs[shortest].remaining--;
        time++;

        if (jobs[shortest].remaining == 0) {
            jobs[shortest].completion_time = time;
            jobs[shortest].finished = 1;
            done++;
        }
    }
}

void calculate_metrics(struct job jobs[], int job_count)
{
    int total_turnaround = 0;
    int total_response = 0;

    for (int i = 0; i < job_count; i++) {
        int turnaround =
            jobs[i].completion_time - jobs[i].arrival;

        int response =
            jobs[i].start_time - jobs[i].arrival;

        total_turnaround += turnaround;
        total_response += response;

        printf(
            "Job %c: Turnaround = %d, Response = %d\n",
            jobs[i].name,
            turnaround,
            response
        );
    }

    printf(
        "Average Turnaround: %.2f\n",
        (double)total_turnaround / job_count
    );

    printf(
        "Average Response: %.2f\n",
        (double)total_response / job_count
    );
}

int main()
{
    // Initialize job set, run simulations;

   struct job jobs[] =
    {
    {'A', 0, 100, 100, -1, 0, 0},
    {'B', 10, 10, 10, -1, 0, 0},
    {'C', 10, 10, 10, -1, 0, 0}
    };

    int job_count = sizeof(jobs) / sizeof(jobs[0]);

    simulate_stcf(jobs, job_count);
    calculate_metrics(jobs, job_count);
    return 0;
}
