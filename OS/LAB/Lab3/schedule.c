#include <stdio.h>

struct job {
    char name;
    int arrival;
    int lenght;
    int start_time;
    int completion_time;
};

void simulate_fifo(struct job jobs[], int job_count)
{
    // FIFO logic here
    int current_time = 0;

    for (int i = 0; i < job_count; i++) {
        jobs[i].start_time = current_time;
        current_time += jobs[i].lenght;
        jobs[i].completion_time = current_time;
    }
}

void simulate_sjf(struct job jobs[], int job_count)
{
    // SJF logic here
    for (int i = 0; i < job_count - 1; i++) {
        for (int j = i + 1; j < job_count; j++) {
            if (jobs[j].lenght < jobs[i].lenght) {
                struct job temp = jobs[i];
                jobs[i] = jobs[j];
                jobs[j] = temp;
            }
        }
    }

    printf("SJF execution order: ");

    for (int i = 0; i < job_count; i++) {
        printf("%c(%d) ", jobs[i].name, jobs[i].lenght);
    }

    printf("\n");
}

int main()
{
    // Initialize job set, run simulations
    struct job jobs[] = {
        {'A', 0, 10, -1, -1},
        {'B', 0, 5, -1, -1},
        {'C', 0, 2, -1, -1}
    };

    int job_count = sizeof(jobs) / sizeof(jobs[0]);

    int total_turnaround = 0, total_response = 0;
    simulate_fifo(jobs, job_count);
    simulate_sjf(jobs, job_count);

    for (int i = 0; i < job_count; i++) {
        int turnaround = jobs[i].completion_time - jobs[i].arrival;
        int response = jobs[i].start_time - jobs[i].arrival;

        total_turnaround += turnaround;
        total_response += response;

        printf(
            "Job %c: Turnaround = %d, Response = %d\n",
            jobs[i].name,
            turnaround,
            response
        );
    }

    printf("average Turnaround: %.2f\n", (double)total_turnaround / job_count);
    printf("Average Response: %.2f\n", (double)total_response / job_count);

    return 0;
}
