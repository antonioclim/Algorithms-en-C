/**
 * =============================================================================
 * EXERCISE 2: ROUND-ROBIN TASK SCHEDULER
 * =============================================================================
 *
 * This file is intentionally written as a full laboratory exercise: it combines
 * (i) an explicit queue implementation (for ready processes) with (ii) a
 * deterministic simulation of round-robin CPU scheduling.
 *
 * The simulation supports a verbose mode (execution trace plus Gantt chart)
 * suitable for interactive study and a minimal test mode that produces stable
 * output for automated checking.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 * Usage:
 *   ./exercise2 <process_file> <time_quantum>
 *   ./exercise2 tests/test2_input.txt 2 --test
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_PROCESSES 100
#define MAX_NAME_LENGTH 32
#define MAX_GANTT_ENTRIES 500
#define PROGRESS_BAR_WIDTH 16

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    int pid;
    char name[MAX_NAME_LENGTH];
    int burst_time;
    int remaining_time;
    int arrival_time;
    int start_time;        /* first time the process is scheduled, -1 if never */
    int completion_time;   /* finish time in the simulated timeline */
    int turnaround_time;   /* completion_time - arrival_time */
    int waiting_time;      /* turnaround_time - burst_time */
} Process;

typedef struct {
    int pid;
    char name[MAX_NAME_LENGTH];
    int start_time;
    int end_time;
} GanttEntry;

/* Queue of indices into the processes array. */
typedef struct {
    int data[MAX_PROCESSES];
    int front;
    int rear;
    int count;
    int capacity;
} ProcessQueue;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

static void pq_init(ProcessQueue *q);
static bool pq_is_empty(const ProcessQueue *q);
static bool pq_enqueue(ProcessQueue *q, int process_index);
static bool pq_dequeue(ProcessQueue *q, int *process_index);

static int load_processes(const char *filename, Process processes[]);
static void print_process_table(const Process processes[], int count);
static void print_progress_bar(int completed, int total);

static void run_scheduler(Process processes[], int count, int quantum,
                          GanttEntry gantt[], int *gantt_count,
                          bool verbose, bool record_gantt);

static void print_gantt_chart(const GanttEntry gantt[], int count);
static void print_statistics(const Process processes[], int count);
static void print_averages_only(const Process processes[], int count);

static int compare_by_arrival_then_pid(const void *a, const void *b);
static void print_usage(const char *program_name);

/* =============================================================================
 * QUEUE OPERATIONS
 * =============================================================================
 */

static void pq_init(ProcessQueue *q) {
    if (!q) {
        return;
    }
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->capacity = MAX_PROCESSES;
}

static bool pq_is_empty(const ProcessQueue *q) {
    return (!q) || (q->count == 0);
}

static bool pq_enqueue(ProcessQueue *q, int process_index) {
    if (!q) {
        return false;
    }
    if (q->count >= q->capacity) {
        return false;
    }

    q->data[q->rear] = process_index;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    return true;
}

static bool pq_dequeue(ProcessQueue *q, int *process_index) {
    if (!q || !process_index) {
        return false;
    }
    if (pq_is_empty(q)) {
        return false;
    }

    *process_index = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return true;
}

/* =============================================================================
 * FILE LOADING
 * =============================================================================
 */

static int load_processes(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }

    int count = 0;
    char line[256];

    while (fgets(line, (int)sizeof(line), file) && count < MAX_PROCESSES) {
        /* Skip comments and empty lines. */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }

        Process *p = &processes[count];
        memset(p, 0, sizeof(*p));

        if (sscanf(line, "%d %31s %d %d",
                   &p->pid, p->name, &p->burst_time, &p->arrival_time) == 4) {
            p->remaining_time = p->burst_time;
            p->start_time = -1;
            p->completion_time = 0;
            p->turnaround_time = 0;
            p->waiting_time = 0;
            count++;
        }
    }

    fclose(file);
    return count;
}

/* =============================================================================
 * DISPLAY FUNCTIONS
 * =============================================================================
 */

static void print_progress_bar(int completed, int total) {
    int filled = (completed * PROGRESS_BAR_WIDTH) / total;

    printf("[");
    for (int i = 0; i < PROGRESS_BAR_WIDTH; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    printf("]");
}

static void print_process_table(const Process processes[], int count) {
    printf("\n  Loaded Processes:\n");
    printf("  ┌─────┬────────────────┬───────────┬───────────┐\n");
    printf("  │ PID │ Name           │ Burst(ms) │ Arrival   │\n");
    printf("  ├─────┼────────────────┼───────────┼───────────┤\n");

    for (int i = 0; i < count; i++) {
        printf("  │ %3d │ %-14s │    %3d    │    %3d    │\n",
               processes[i].pid,
               processes[i].name,
               processes[i].burst_time,
               processes[i].arrival_time);
    }

    printf("  └─────┴────────────────┴───────────┴───────────┘\n\n");
}

/* =============================================================================
 * SCHEDULER IMPLEMENTATION
 * =============================================================================
 */

static void run_scheduler(Process processes[], int count, int quantum,
                          GanttEntry gantt[], int *gantt_count,
                          bool verbose, bool record_gantt) {
    ProcessQueue ready_queue;
    pq_init(&ready_queue);

    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;

    /*
     * in_queue[i] is used as a duplicate suppressor for the arrival stage.
     * A process is marked true when it has been admitted to the ready queue at
     * least once and remains true until completion.
     */
    bool in_queue[MAX_PROCESSES] = {false};

    if (gantt_count) {
        *gantt_count = 0;
    }

    if (verbose) {
        printf("  === Execution Timeline ===\n");
        printf("  ─────────────────────────────────────────────────────────────\n");
    }

    /* Admit processes that arrive at time 0. */
    while (next_arrival_idx < count && processes[next_arrival_idx].arrival_time <= current_time) {
        pq_enqueue(&ready_queue, next_arrival_idx);
        in_queue[next_arrival_idx] = true;
        next_arrival_idx++;
    }

    while (completed < count) {
        /* Admit newly arrived processes. */
        while (next_arrival_idx < count && processes[next_arrival_idx].arrival_time <= current_time) {
            if (!in_queue[next_arrival_idx]) {
                pq_enqueue(&ready_queue, next_arrival_idx);
                in_queue[next_arrival_idx] = true;
            }
            next_arrival_idx++;
        }

        /* If no ready process exists, jump to the next arrival. */
        if (pq_is_empty(&ready_queue)) {
            if (next_arrival_idx < count) {
                current_time = processes[next_arrival_idx].arrival_time;
                continue;
            }
            break;
        }

        int idx;
        pq_dequeue(&ready_queue, &idx);
        Process *p = &processes[idx];

        if (p->start_time == -1) {
            p->start_time = current_time;
        }

        int run_time = (p->remaining_time < quantum) ? p->remaining_time : quantum;

        if (record_gantt && gantt && gantt_count && *gantt_count < MAX_GANTT_ENTRIES) {
            gantt[*gantt_count].pid = p->pid;
            strncpy(gantt[*gantt_count].name, p->name, MAX_NAME_LENGTH - 1);
            gantt[*gantt_count].name[MAX_NAME_LENGTH - 1] = '\0';
            gantt[*gantt_count].start_time = current_time;
            gantt[*gantt_count].end_time = current_time + run_time;
            (*gantt_count)++;
        }

        if (verbose) {
            printf("  │ [%3d-%3d] %-12s ", current_time, current_time + run_time, p->name);
            print_progress_bar(p->burst_time - p->remaining_time + run_time, p->burst_time);
        }

        current_time += run_time;
        p->remaining_time -= run_time;

        /* Admit processes that arrive during the execution slice. */
        while (next_arrival_idx < count && processes[next_arrival_idx].arrival_time <= current_time) {
            if (!in_queue[next_arrival_idx]) {
                pq_enqueue(&ready_queue, next_arrival_idx);
                in_queue[next_arrival_idx] = true;
            }
            next_arrival_idx++;
        }

        if (p->remaining_time > 0) {
            if (verbose) {
                printf(" (%d remaining)\n", p->remaining_time);
            }
            pq_enqueue(&ready_queue, idx);
        } else {
            if (verbose) {
                printf(" ✓ DONE\n");
            }
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            completed++;
        }
    }

    if (verbose) {
        printf("  ─────────────────────────────────────────────────────────────\n\n");
    }
}

static void print_gantt_chart(const GanttEntry gantt[], int count) {
    if (count <= 0) {
        return;
    }

    printf("  === Gantt Chart ===\n  ");

    /* Top border. */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        for (int j = 0; j < chars + 2; j++) {
            printf("─");
        }
    }
    printf("\n  ");

    /* Process names. */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        printf("|%-*.*s", chars + 1, chars + 1, gantt[i].name);
    }
    printf("|\n  ");

    /* Bottom border. */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        for (int j = 0; j < chars + 2; j++) {
            printf("─");
        }
    }
    printf("\n  ");

    /* Time markers. */
    printf("%d", gantt[0].start_time);
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        printf("%*d", chars + 2, gantt[i].end_time);
    }

    if (count > 15) {
        printf("\n  ... (%d more entries)", count - 15);
    }

    printf("\n\n");
}

static void print_statistics(const Process processes[], int count) {
    printf("  === Process Statistics ===\n");
    printf("  ┌─────┬────────────────┬───────┬────────┬─────────┬────────┐\n");
    printf("  │ PID │ Name           │ Burst │ Finish │   TAT   │  Wait  │\n");
    printf("  ├─────┼────────────────┼───────┼────────┼─────────┼────────┤\n");

    float total_tat = 0;
    float total_wait = 0;

    for (int i = 0; i < count; i++) {
        printf("  │ %3d │ %-14s │  %3d  │  %4d  │   %3d   │  %3d   │\n",
               processes[i].pid,
               processes[i].name,
               processes[i].burst_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time);

        total_tat += processes[i].turnaround_time;
        total_wait += processes[i].waiting_time;
    }

    printf("  └─────┴────────────────┴───────┴────────┴─────────┴────────┘\n\n");

    if (count > 0) {
        printf("  Average Turnaround Time: %.2f ms\n", total_tat / count);
        printf("  Average Waiting Time:    %.2f ms\n", total_wait / count);
    }
}

static void print_averages_only(const Process processes[], int count) {
    double total_tat = 0.0;
    double total_wait = 0.0;

    for (int i = 0; i < count; i++) {
        total_tat += processes[i].turnaround_time;
        total_wait += processes[i].waiting_time;
    }

    if (count > 0) {
        printf("  Average Turnaround Time: %.2f ms\n", total_tat / (double)count);
        printf("  Average Waiting Time:    %.2f ms\n", total_wait / (double)count);
    }
}

/* =============================================================================
 * HELPERS
 * =============================================================================
 */

static int compare_by_arrival_then_pid(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;

    if (p1->arrival_time < p2->arrival_time) {
        return -1;
    }
    if (p1->arrival_time > p2->arrival_time) {
        return 1;
    }

    if (p1->pid < p2->pid) {
        return -1;
    }
    if (p1->pid > p2->pid) {
        return 1;
    }

    return 0;
}

static void print_usage(const char *program_name) {
    printf("Usage: %s <process_file> <time_quantum> [--test]\n", program_name);
    printf("\nArguments:\n");
    printf("  process_file   Path to file containing process definitions\n");
    printf("  time_quantum   Time slice in milliseconds (positive integer)\n");
    printf("  --test         Emit minimal deterministic output for automation\n");
    printf("\nExample:\n");
    printf("  %s ../data/processes.txt 3\n", program_name);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    bool test_mode = false;

    if (argc == 4 && strcmp(argv[3], "--test") == 0) {
        test_mode = true;
    }

    if (argc != 3 && argc != 4) {
        if (!test_mode) {
            printf("\n");
            printf("╔═══════════════════════════════════════════════════════════════╗\n");
            printf("║     EXERCISE 2: ROUND-ROBIN TASK SCHEDULER                    ║\n");
            printf("╚═══════════════════════════════════════════════════════════════╝\n");
        }
        print_usage(argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int quantum = atoi(argv[2]);

    if (quantum <= 0) {
        fprintf(stderr, "Error: Time quantum must be a positive integer\n");
        return 1;
    }

    Process processes[MAX_PROCESSES];
    int process_count = load_processes(filename, processes);

    if (process_count <= 0) {
        fprintf(stderr, "Error: No processes loaded\n");
        return 1;
    }

    if (!test_mode) {
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║     EXERCISE 2: ROUND-ROBIN TASK SCHEDULER                    ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n");

        printf("\n  Loaded %d processes from '%s'\n", process_count, filename);
        printf("  Time Quantum: %d ms\n", quantum);
    } else {
        /* In test mode we avoid a leading blank line or decorative banners. */
        printf("  Loaded %d processes from '%s'\n", process_count, filename);
        printf("  Time Quantum: %d ms\n", quantum);
    }

    /* Deterministic ordering: sort by arrival time then by PID. */
    qsort(processes, (size_t)process_count, sizeof(Process), compare_by_arrival_then_pid);

    print_process_table(processes, process_count);

    GanttEntry gantt[MAX_GANTT_ENTRIES];
    int gantt_count = 0;

    if (test_mode) {
        run_scheduler(processes, process_count, quantum, gantt, &gantt_count, false, false);
        print_averages_only(processes, process_count);
        return 0;
    }

    run_scheduler(processes, process_count, quantum, gantt, &gantt_count, true, true);
    print_gantt_chart(gantt, gantt_count);
    print_statistics(processes, process_count);

    printf("\n");
    return 0;
}
