/**
 * =============================================================================
 * HOMEWORK 2: PRINT QUEUE MANAGER - SOLUTION
 * =============================================================================
 *
 * Complete solution for multi-priority print queue management system.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * Usage: ./homework2_sol <jobs_file>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_JOBS 100
#define MAX_FILENAME_LENGTH 64
#define QUEUE_CAPACITY 50

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_MEDIUM = 1,
    PRIORITY_LOW = 2,
    PRIORITY_COUNT = 3
} Priority;

const char *PRIORITY_NAMES[] = {"HIGH", "MEDIUM", "LOW"};

typedef struct {
    int job_id;
    char filename[MAX_FILENAME_LENGTH];
    int pages;
    Priority priority;
    int arrival_time;
    int start_time;
    int completion_time;
} PrintJob;

typedef struct {
    int data[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
} JobQueue;

typedef struct {
    int jobs_processed;
    int total_wait_time[PRIORITY_COUNT];
    int job_count[PRIORITY_COUNT];
    int max_queue_length[PRIORITY_COUNT];
} QueueStats;

/* =============================================================================
 * QUEUE IMPLEMENTATION
 * =============================================================================
 */

void jq_init(JobQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

bool jq_is_empty(const JobQueue *q) {
    return q->count == 0;
}

bool jq_enqueue(JobQueue *q, int job_index) {
    if (q->count >= QUEUE_CAPACITY) return false;
    
    q->data[q->rear] = job_index;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;
    return true;
}

bool jq_dequeue(JobQueue *q, int *job_index) {
    if (jq_is_empty(q)) return false;
    
    *job_index = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    return true;
}

int jq_size(const JobQueue *q) {
    return q->count;
}

/* =============================================================================
 * MULTI-QUEUE SYSTEM
 * =============================================================================
 */

typedef struct {
    JobQueue queues[PRIORITY_COUNT];
    QueueStats stats;
} PrintQueueSystem;

void pqs_init(PrintQueueSystem *pqs) {
    for (int i = 0; i < PRIORITY_COUNT; i++) {
        jq_init(&pqs->queues[i]);
    }
    memset(&pqs->stats, 0, sizeof(QueueStats));
}

bool pqs_add_job(PrintQueueSystem *pqs, int job_index, Priority priority) {
    if (priority < 0 || priority >= PRIORITY_COUNT) return false;
    
    bool success = jq_enqueue(&pqs->queues[priority], job_index);
    
    if (success) {
        int current_size = jq_size(&pqs->queues[priority]);
        if (current_size > pqs->stats.max_queue_length[priority]) {
            pqs->stats.max_queue_length[priority] = current_size;
        }
    }
    
    return success;
}

int pqs_get_next_job(PrintQueueSystem *pqs) {
    int job_index;
    
    /* Check queues in priority order */
    for (int p = PRIORITY_HIGH; p < PRIORITY_COUNT; p++) {
        if (jq_dequeue(&pqs->queues[p], &job_index)) {
            return job_index;
        }
    }
    
    return -1;  /* No jobs available */
}

bool pqs_has_jobs(const PrintQueueSystem *pqs) {
    for (int p = 0; p < PRIORITY_COUNT; p++) {
        if (!jq_is_empty(&pqs->queues[p])) return true;
    }
    return false;
}

/* =============================================================================
 * FILE LOADING
 * =============================================================================
 */

Priority parse_priority(const char *str) {
    if (strcmp(str, "HIGH") == 0) return PRIORITY_HIGH;
    if (strcmp(str, "MEDIUM") == 0) return PRIORITY_MEDIUM;
    if (strcmp(str, "LOW") == 0) return PRIORITY_LOW;
    return PRIORITY_MEDIUM;  /* Default */
}

int load_jobs(const char *filename, PrintJob jobs[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    int count = 0;
    char line[256];
    char priority_str[16];
    
    while (fgets(line, sizeof(line), file) && count < MAX_JOBS) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
        
        PrintJob *job = &jobs[count];
        
        if (sscanf(line, "%d %63s %d %15s %d",
                   &job->job_id, job->filename, &job->pages, 
                   priority_str, &job->arrival_time) == 5) {
            job->priority = parse_priority(priority_str);
            job->start_time = -1;
            job->completion_time = -1;
            count++;
        }
    }
    
    fclose(file);
    return count;
}

/* =============================================================================
 * COMPARISON FUNCTION FOR SORTING
 * =============================================================================
 */

int compare_by_arrival(const void *a, const void *b) {
    const PrintJob *j1 = (const PrintJob *)a;
    const PrintJob *j2 = (const PrintJob *)b;
    return j1->arrival_time - j2->arrival_time;
}

/* =============================================================================
 * SIMULATION
 * =============================================================================
 */

void run_simulation(PrintJob jobs[], int job_count, PrintQueueSystem *pqs) {
    int current_time = 0;
    int next_job_idx = 0;
    int completed = 0;
    int current_job = -1;
    int pages_remaining = 0;
    
    printf("\n  === Print Queue Simulation ===\n");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    /* Sort jobs by arrival time */
    qsort(jobs, job_count, sizeof(PrintJob), compare_by_arrival);
    
    while (completed < job_count || current_job != -1) {
        /* Add newly arrived jobs to queues */
        while (next_job_idx < job_count && 
               jobs[next_job_idx].arrival_time <= current_time) {
            PrintJob *job = &jobs[next_job_idx];
            printf("  [Time %3d] Job %d (%s) arrived - Priority: %s\n",
                   current_time, job->job_id, job->filename,
                   PRIORITY_NAMES[job->priority]);
            pqs_add_job(pqs, next_job_idx, job->priority);
            next_job_idx++;
        }
        
        /* If no current job, get next from queue */
        if (current_job == -1) {
            current_job = pqs_get_next_job(pqs);
            
            if (current_job != -1) {
                PrintJob *job = &jobs[current_job];
                job->start_time = current_time;
                pages_remaining = job->pages;
                printf("  [Time %3d] Started printing Job %d (%s)\n",
                       current_time, job->job_id, job->filename);
            } else if (next_job_idx < job_count) {
                /* No jobs ready, jump to next arrival */
                current_time = jobs[next_job_idx].arrival_time;
                continue;
            } else {
                break;  /* No more jobs */
            }
        }
        
        /* Simulate one time unit of printing */
        if (current_job != -1) {
            pages_remaining--;
            current_time++;
            
            /* Check if job completed */
            if (pages_remaining == 0) {
                PrintJob *job = &jobs[current_job];
                job->completion_time = current_time;
                
                int wait_time = job->start_time - job->arrival_time;
                pqs->stats.total_wait_time[job->priority] += wait_time;
                pqs->stats.job_count[job->priority]++;
                pqs->stats.jobs_processed++;
                
                printf("  [Time %3d] Completed Job %d (%s)\n",
                       current_time, job->job_id, job->filename);
                
                completed++;
                current_job = -1;
            }
        }
    }
    
    printf("  ─────────────────────────────────────────────────────────────\n\n");
}

/* =============================================================================
 * STATISTICS
 * =============================================================================
 */

void print_statistics(const PrintQueueSystem *pqs, const PrintJob jobs[], int job_count) {
    printf("  === Simulation Statistics ===\n");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    /* Find total simulation time */
    int max_completion = 0;
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].completion_time > max_completion) {
            max_completion = jobs[i].completion_time;
        }
    }
    
    printf("  Total simulation time: %d units\n", max_completion);
    printf("  Jobs processed: %d\n\n", pqs->stats.jobs_processed);
    
    printf("  Queue Statistics:\n");
    printf("  ┌──────────┬───────────┬────────────────┬──────────────┐\n");
    printf("  │ Priority │ Jobs      │ Avg Wait Time  │ Max Queue    │\n");
    printf("  ├──────────┼───────────┼────────────────┼──────────────┤\n");
    
    for (int p = 0; p < PRIORITY_COUNT; p++) {
        float avg_wait = 0;
        if (pqs->stats.job_count[p] > 0) {
            avg_wait = (float)pqs->stats.total_wait_time[p] / pqs->stats.job_count[p];
        }
        
        printf("  │ %-8s │    %3d    │     %6.2f     │      %3d     │\n",
               PRIORITY_NAMES[p],
               pqs->stats.job_count[p],
               avg_wait,
               pqs->stats.max_queue_length[p]);
    }
    
    printf("  └──────────┴───────────┴────────────────┴──────────────┘\n\n");
    
    /* Overall averages */
    float total_wait = 0;
    int total_jobs = 0;
    for (int p = 0; p < PRIORITY_COUNT; p++) {
        total_wait += pqs->stats.total_wait_time[p];
        total_jobs += pqs->stats.job_count[p];
    }
    
    if (total_jobs > 0) {
        printf("  Overall average wait time: %.2f units\n", total_wait / total_jobs);
    }
    
    /* Job details */
    printf("\n  Job Completion Details:\n");
    printf("  ┌─────┬────────────────────────┬───────┬──────────┬────────┐\n");
    printf("  │ ID  │ Filename               │ Pages │ Priority │ Wait   │\n");
    printf("  ├─────┼────────────────────────┼───────┼──────────┼────────┤\n");
    
    for (int i = 0; i < job_count; i++) {
        const PrintJob *job = &jobs[i];
        int wait = job->start_time - job->arrival_time;
        printf("  │ %3d │ %-22s │  %3d  │ %-8s │  %4d  │\n",
               job->job_id, job->filename, job->pages,
               PRIORITY_NAMES[job->priority], wait);
    }
    
    printf("  └─────┴────────────────────────┴───────┴──────────┴────────┘\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 2: PRINT QUEUE MANAGER - SOLUTION                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    if (argc != 2) {
        printf("Usage: %s <jobs_file>\n", argv[0]);
        printf("\nExample jobs file format:\n");
        printf("  # JobID Filename Pages Priority ArrivalTime\n");
        printf("  1 report.pdf 10 HIGH 0\n");
        printf("  2 image.png 2 LOW 0\n");
        return 1;
    }
    
    /* Load jobs */
    PrintJob jobs[MAX_JOBS];
    int job_count = load_jobs(argv[1], jobs);
    
    if (job_count <= 0) {
        fprintf(stderr, "Error: No jobs loaded from '%s'\n", argv[1]);
        return 1;
    }
    
    printf("\n  Loaded %d print jobs from '%s'\n", job_count, argv[1]);
    
    /* Initialise print queue system */
    PrintQueueSystem pqs;
    pqs_init(&pqs);
    
    /* Run simulation */
    run_simulation(jobs, job_count, &pqs);
    
    /* Print statistics */
    print_statistics(&pqs, jobs, job_count);
    
    printf("\n");
    return 0;
}
