/**
 * =============================================================================
 * EXERCISE 2: ROUND-ROBIN TASK SCHEDULER
 * =============================================================================
 *
 * OBJECTIVE:
 *   Build a round-robin CPU scheduler simulation using a queue of processes.
 *   The scheduler should execute processes in time slices (quanta), re-queuing
 *   unfinished processes and tracking comprehensive statistics.
 *
 * REQUIREMENTS:
 *   1. Define a Process structure with PID, name, burst time and state tracking
 *   2. Implement a queue specifically for Process pointers
 *   3. Load process data from a file
 *   4. Implement the round-robin scheduling algorithm
 *   5. Generate a text-based Gantt chart of execution order
 *   6. Calculate turnaround time and waiting time for each process
 *   7. Compute and display average statistics
 *   8. Support adding new processes during simulation (optional bonus)
 *
 * INPUT FILE FORMAT (processes.txt):
 *   <PID> <ProcessName> <BurstTime> <ArrivalTime>
 *   1 Chrome 10 0
 *   2 VSCode 6 0
 *   3 Spotify 8 2
 *   4 Terminal 4 3
 *
 * COMMAND LINE:
 *   ./exercise2 <process_file> <time_quantum>
 *   ./exercise2 ../data/processes.txt 3
 *
 * EXPECTED OUTPUT:
 *   Loaded 4 processes from file
 *   Time Quantum: 3 ms
 *   
 *   === Execution Timeline ===
 *   [0-3]   Chrome    ████████░░░░░░░░ (7 remaining)
 *   [3-6]   VSCode    ████████████████ (3 remaining)
 *   [6-9]   Spotify   ██████░░░░░░░░░░ (5 remaining)
 *   ...
 *   
 *   === Gantt Chart ===
 *   |Chrome|VSCode|Spotify|Terminal|Chrome|...
 *   0      3      6       9        12     ...
 *   
 *   === Process Statistics ===
 *   PID  Name      Burst  Finish  TAT   Wait
 *   1    Chrome    10     22      22    12
 *   2    VSCode    6      15      15    9
 *   ...
 *   
 *   Average Turnaround Time: 17.25 ms
 *   Average Waiting Time: 10.50 ms
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
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

/**
 * TODO 1: Define the Process structure
 *
 * The structure should contain:
 *   - pid: unique process identifier (int)
 *   - name: process name (char array of MAX_NAME_LENGTH)
 *   - burst_time: total CPU time required (int)
 *   - remaining_time: CPU time still needed (int)
 *   - arrival_time: when the process arrived (int)
 *   - start_time: when first executed, -1 if not started (int)
 *   - completion_time: when process finished (int)
 *   - turnaround_time: completion - arrival (int)
 *   - waiting_time: turnaround - burst (int)
 *
 * Hint: Initialise start_time to -1 to detect first execution
 */
typedef struct {
    /* YOUR CODE HERE */
    int pid;
    char name[MAX_NAME_LENGTH];
    int burst_time;
    int remaining_time;
    int arrival_time;
    /* Add more fields */
} Process;

/**
 * Gantt chart entry for visualisation.
 */
typedef struct {
    int pid;
    char name[MAX_NAME_LENGTH];
    int start_time;
    int end_time;
} GanttEntry;

/**
 * TODO 2: Define the ProcessQueue structure
 *
 * A queue to hold process indices (not the processes themselves).
 * Using indices allows us to reference the original process array.
 *
 * Fields needed:
 *   - data: array of integers (process indices)
 *   - front: front index
 *   - rear: rear index
 *   - count: current number of elements
 *   - capacity: maximum capacity
 */
typedef struct {
    /* YOUR CODE HERE */
    int data[MAX_PROCESSES];
    int front;
    int rear;
    int count;
} ProcessQueue;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

void pq_init(ProcessQueue *q);
bool pq_is_empty(const ProcessQueue *q);
bool pq_enqueue(ProcessQueue *q, int process_index);
bool pq_dequeue(ProcessQueue *q, int *process_index);
int pq_size(const ProcessQueue *q);

int load_processes(const char *filename, Process processes[]);
void print_process_table(const Process processes[], int count);
void print_progress_bar(int completed, int total);
void run_scheduler(Process processes[], int count, int quantum, GanttEntry gantt[], int *gantt_count);
void print_gantt_chart(const GanttEntry gantt[], int count);
void print_statistics(const Process processes[], int count);

/* =============================================================================
 * QUEUE OPERATIONS
 * =============================================================================
 */

/**
 * TODO 3: Initialise the process queue
 *
 * @param q  Pointer to queue to initialise
 *
 * Set front, rear and count to 0.
 */
void pq_init(ProcessQueue *q) {
    /* YOUR CODE HERE */
}

/**
 * TODO 4: Check if queue is empty
 *
 * @param q  Pointer to queue
 * @return   true if empty, false otherwise
 */
bool pq_is_empty(const ProcessQueue *q) {
    /* YOUR CODE HERE */
    return true;  /* Replace this */
}

/**
 * TODO 5: Enqueue a process index
 *
 * @param q              Pointer to queue
 * @param process_index  Index of process in the processes array
 * @return               true on success, false if full
 *
 * Steps:
 *   1. Check if queue is full (count >= MAX_PROCESSES)
 *   2. Store process_index at data[rear]
 *   3. Increment rear with wraparound: (rear + 1) % MAX_PROCESSES
 *   4. Increment count
 *   5. Return true
 */
bool pq_enqueue(ProcessQueue *q, int process_index) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/**
 * TODO 6: Dequeue a process index
 *
 * @param q              Pointer to queue
 * @param process_index  Pointer to store the dequeued index
 * @return               true on success, false if empty
 *
 * Steps:
 *   1. Check if queue is empty
 *   2. Store data[front] in *process_index
 *   3. Increment front with wraparound
 *   4. Decrement count
 *   5. Return true
 */
bool pq_dequeue(ProcessQueue *q, int *process_index) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/**
 * Return queue size.
 */
int pq_size(const ProcessQueue *q) {
    return q->count;
}

/* =============================================================================
 * FILE LOADING
 * =============================================================================
 */

/**
 * TODO 7: Load processes from file
 *
 * @param filename   Path to the process data file
 * @param processes  Array to store loaded processes
 * @return           Number of processes loaded, or -1 on error
 *
 * File format per line: <PID> <Name> <BurstTime> <ArrivalTime>
 *
 * Steps:
 *   1. Open the file for reading
 *   2. Return -1 if file cannot be opened
 *   3. Read each line using fscanf or fgets+sscanf
 *   4. For each line:
 *      a. Parse PID, name, burst_time, arrival_time
 *      b. Set remaining_time = burst_time
 *      c. Set start_time = -1 (not yet started)
 *      d. Initialise completion_time, turnaround_time, waiting_time to 0
 *   5. Close the file
 *   6. Return the count of processes loaded
 *
 * Hint: Use fscanf(file, "%d %s %d %d", ...) for parsing
 */
int load_processes(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return -1;
    }
    
    int count = 0;
    
    /* YOUR CODE HERE */
    /* Read processes from file */
    
    fclose(file);
    return count;
}

/* =============================================================================
 * DISPLAY FUNCTIONS
 * =============================================================================
 */

/**
 * Print a visual progress bar.
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

/**
 * Print process table before scheduling.
 */
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

/**
 * TODO 8: Implement the round-robin scheduler
 *
 * @param processes    Array of processes
 * @param count        Number of processes
 * @param quantum      Time quantum (time slice) in ms
 * @param gantt        Array to store Gantt chart entries
 * @param gantt_count  Pointer to store number of Gantt entries
 *
 * Algorithm:
 *   1. Initialise ready queue
 *   2. Sort processes by arrival time (optional, assume already sorted)
 *   3. Set current_time = 0, completed = 0, next_arrival = 0
 *   4. Add processes that have arrived (arrival_time <= current_time) to queue
 *   5. While completed < count:
 *      a. Add any newly arrived processes to queue
 *      b. If queue is empty but processes remain, jump to next arrival
 *      c. Dequeue a process index
 *      d. If this is first execution, record start_time
 *      e. Calculate run_time = min(remaining_time, quantum)
 *      f. Record Gantt entry (pid, name, current_time, current_time + run_time)
 *      g. Print execution line with progress bar
 *      h. Update current_time += run_time
 *      i. Update remaining_time -= run_time
 *      j. Add any processes that arrived during execution
 *      k. If remaining_time > 0, re-enqueue the process
 *      l. Else, record completion_time, calculate TAT and wait, increment completed
 *
 * Hint: Track which processes have been added to queue to avoid duplicates
 */
void run_scheduler(Process processes[], int count, int quantum, 
                   GanttEntry gantt[], int *gantt_count) {
    ProcessQueue ready_queue;
    pq_init(&ready_queue);
    
    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;  /* Index of next process to arrive */
    bool in_queue[MAX_PROCESSES] = {false};  /* Track which processes are queued */
    
    *gantt_count = 0;
    
    printf("  === Execution Timeline ===\n");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    /* YOUR CODE HERE */
    /* Implement the round-robin scheduling algorithm */
    
    printf("  ─────────────────────────────────────────────────────────────\n\n");
}

/**
 * TODO 9: Print the Gantt chart
 *
 * @param gantt  Array of Gantt entries
 * @param count  Number of entries
 *
 * Display format:
 *   |Process1|Process2|Process3|...
 *   0        3        6        9
 *
 * Steps:
 *   1. Print top border
 *   2. For each entry, print process name with appropriate width
 *   3. Print bottom border with time markers
 */
void print_gantt_chart(const GanttEntry gantt[], int count) {
    if (count == 0) return;
    
    printf("  === Gantt Chart ===\n  ");
    
    /* YOUR CODE HERE */
    /* Print the Gantt chart visualisation */
    
    printf("\n\n");
}

/**
 * TODO 10: Print final statistics
 *
 * @param processes  Array of processes (with computed times)
 * @param count      Number of processes
 *
 * Display a table with:
 *   - PID, Name, Burst, Completion, Turnaround, Waiting times
 *   - Average turnaround and waiting times
 */
void print_statistics(const Process processes[], int count) {
    printf("  === Process Statistics ===\n");
    printf("  ┌─────┬────────────────┬───────┬────────┬─────────┬────────┐\n");
    printf("  │ PID │ Name           │ Burst │ Finish │   TAT   │  Wait  │\n");
    printf("  ├─────┼────────────────┼───────┼────────┼─────────┼────────┤\n");
    
    float total_tat = 0;
    float total_wait = 0;
    
    /* YOUR CODE HERE */
    /* Print statistics for each process */
    /* Calculate totals for averages */
    
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

/**
 * Compare processes by arrival time for sorting.
 */
int compare_by_arrival(const void *a, const void *b) {
    const Process *p1 = (const Process *)a;
    const Process *p2 = (const Process *)b;
    return p1->arrival_time - p2->arrival_time;
}

/**
 * Print usage information.
 */
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
    printf("║     EXERCISE 2: ROUND-ROBIN TASK SCHEDULER                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Check command line arguments */
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
    
    /* Load processes from file */
    Process processes[MAX_PROCESSES];
    int process_count = load_processes(filename, processes);
    
    if (process_count <= 0) {
        fprintf(stderr, "Error: No processes loaded\n");
        return 1;
    }
    
    printf("\n  Loaded %d processes from '%s'\n", process_count, filename);
    printf("  Time Quantum: %d ms\n", quantum);
    
    /* Sort by arrival time */
    qsort(processes, process_count, sizeof(Process), compare_by_arrival);
    
    /* Display loaded processes */
    print_process_table(processes, process_count);
    
    /* Run the scheduler */
    GanttEntry gantt[MAX_GANTT_ENTRIES];
    int gantt_count = 0;
    
    run_scheduler(processes, process_count, quantum, gantt, &gantt_count);
    
    /* Print Gantt chart */
    print_gantt_chart(gantt, gantt_count);
    
    /* Print statistics */
    print_statistics(processes, process_count);
    
    printf("\n");
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add support for process priorities. Implement a multi-level feedback
 *    queue where processes can move between priority levels based on their
 *    behaviour.
 *
 * 2. Implement dynamic process arrival. Allow new processes to be added
 *    from stdin during simulation by checking for input between time slices.
 *
 * 3. Add preemption support. When a higher-priority process arrives, it
 *    should interrupt the current process immediately.
 *
 * 4. Implement different scheduling algorithms (FCFS, SJF, Priority) and
 *    allow the user to choose which one to use via command line.
 *
 * 5. Add a graphical timeline output that shows parallel execution on
 *    multiple CPUs (simulate multi-core scheduling).
 *
 * =============================================================================
 */
