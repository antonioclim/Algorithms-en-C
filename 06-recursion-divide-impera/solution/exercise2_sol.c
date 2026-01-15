/**
 * =============================================================================
 * EXERCISE 2: ROUND-ROBIN TASK SCHEDULER - SOLUTION
 * =============================================================================
 *
 * Complete solution demonstrating round-robin CPU scheduling using queues.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * Usage: ./exercise2_sol <process_file> <time_quantum>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

typedef struct {
    int pid;
    char name[MAX_NAME_LENGTH];
    int start_time;
    int end_time;
} GanttEntry;

typedef struct {
    int data[MAX_PROCESSES];
    int front;
    int rear;
    int count;
} ProcessQueue;

/* =============================================================================
 * QUEUE IMPLEMENTATION
 * =============================================================================
 */

void pq_init(ProcessQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

bool pq_is_empty(const ProcessQueue *q) {
    return q->count == 0;
}

bool pq_enqueue(ProcessQueue *q, int process_index) {
    if (q->count >= MAX_PROCESSES) return false;
    
    q->data[q->rear] = process_index;
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->count++;
    return true;
}

bool pq_dequeue(ProcessQueue *q, int *process_index) {
    if (pq_is_empty(q)) return false;
    
    *process_index = q->data[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->count--;
    return true;
}

int pq_size(const ProcessQueue *q) {
    return q->count;
}

/* =============================================================================
 * FILE LOADING
 * =============================================================================
 */

int load_processes(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    int count = 0;
    char line[256];
    
    while (fgets(line, sizeof(line), file) && count < MAX_PROCESSES) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        Process *p = &processes[count];
        
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

void print_progress_bar(int completed, int total) {
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

void print_process_table(const Process processes[], int count) {
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

void run_scheduler(Process processes[], int count, int quantum, 
                   GanttEntry gantt[], int *gantt_count) {
    ProcessQueue ready_queue;
    pq_init(&ready_queue);
    
    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    bool in_queue[MAX_PROCESSES] = {false};
    
    *gantt_count = 0;
    
    printf("  === Execution Timeline ===\n");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    /* Add processes that arrive at time 0 */
    while (next_arrival_idx < count && processes[next_arrival_idx].arrival_time <= current_time) {
        pq_enqueue(&ready_queue, next_arrival_idx);
        in_queue[next_arrival_idx] = true;
        next_arrival_idx++;
    }
    
    while (completed < count) {
        /* Check for new arrivals */
        while (next_arrival_idx < count && 
               processes[next_arrival_idx].arrival_time <= current_time) {
            if (!in_queue[next_arrival_idx]) {
                pq_enqueue(&ready_queue, next_arrival_idx);
                in_queue[next_arrival_idx] = true;
            }
            next_arrival_idx++;
        }
        
        /* If queue is empty but processes remain, jump to next arrival */
        if (pq_is_empty(&ready_queue) && completed < count) {
            if (next_arrival_idx < count) {
                current_time = processes[next_arrival_idx].arrival_time;
                continue;
            }
            break;
        }
        
        /* Get next process */
        int idx;
        pq_dequeue(&ready_queue, &idx);
        Process *p = &processes[idx];
        
        /* Record start time if first execution */
        if (p->start_time == -1) {
            p->start_time = current_time;
        }
        
        /* Calculate run time */
        int run_time = (p->remaining_time < quantum) ? p->remaining_time : quantum;
        
        /* Record Gantt entry */
        if (*gantt_count < MAX_GANTT_ENTRIES) {
            gantt[*gantt_count].pid = p->pid;
            strncpy(gantt[*gantt_count].name, p->name, MAX_NAME_LENGTH - 1);
            gantt[*gantt_count].start_time = current_time;
            gantt[*gantt_count].end_time = current_time + run_time;
            (*gantt_count)++;
        }
        
        /* Print execution info */
        printf("  │ [%3d-%3d] %-12s ", current_time, current_time + run_time, p->name);
        print_progress_bar(p->burst_time - p->remaining_time + run_time, p->burst_time);
        
        /* Update times */
        current_time += run_time;
        p->remaining_time -= run_time;
        
        /* Check for new arrivals during execution */
        while (next_arrival_idx < count && 
               processes[next_arrival_idx].arrival_time <= current_time) {
            if (!in_queue[next_arrival_idx]) {
                pq_enqueue(&ready_queue, next_arrival_idx);
                in_queue[next_arrival_idx] = true;
            }
            next_arrival_idx++;
        }
        
        if (p->remaining_time > 0) {
            printf(" (%d remaining)\n", p->remaining_time);
            pq_enqueue(&ready_queue, idx);
        } else {
            printf(" ✓ DONE\n");
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            in_queue[idx] = false;
            completed++;
        }
    }
    
    printf("  ─────────────────────────────────────────────────────────────\n\n");
}

void print_gantt_chart(const GanttEntry gantt[], int count) {
    if (count == 0) return;
    
    printf("  === Gantt Chart ===\n  ");
    
    /* Print top border */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        for (int j = 0; j < chars + 2; j++) printf("─");
    }
    printf("\n  ");
    
    /* Print process names */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        printf("|%-*.*s", chars + 1, chars + 1, gantt[i].name);
    }
    printf("|\n  ");
    
    /* Print bottom border with times */
    for (int i = 0; i < count && i < 15; i++) {
        int width = gantt[i].end_time - gantt[i].start_time;
        int chars = (width < 3) ? 3 : width;
        for (int j = 0; j < chars + 2; j++) printf("─");
    }
    printf("\n  ");
    
    /* Print time markers */
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

void print_statistics(const Process processes[], int count) {
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

/* =============================================================================
 * HELPER FUNCTIONS
 * =============================================================================
 */

int compare_by_arrival(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    return p1->arrival_time - p2->arrival_time;
}

void print_usage(const char *program_name) {
    printf("Usage: %s <process_file> <time_quantum>\n", program_name);
    printf("\nArguments:\n");
    printf("  process_file   Path to file containing process definitions\n");
    printf("  time_quantum   Time slice in milliseconds (positive integer)\n");
    printf("\nExample:\n");
    printf("  %s ../data/processes.txt 3\n", program_name);
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 2: ROUND-ROBIN TASK SCHEDULER - SOLUTION         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    if (argc != 3) {
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
    
    printf("\n  Loaded %d processes from '%s'\n", process_count, filename);
    printf("  Time Quantum: %d ms\n", quantum);
    
    qsort(processes, process_count, sizeof(Process), compare_by_arrival);
    
    print_process_table(processes, process_count);
    
    GanttEntry gantt[MAX_GANTT_ENTRIES];
    int gantt_count = 0;
    
    run_scheduler(processes, process_count, quantum, gantt, &gantt_count);
    
    print_gantt_chart(gantt, gantt_count);
    
    print_statistics(processes, process_count);
    
    printf("\n");
    return 0;
}
