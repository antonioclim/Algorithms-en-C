/**
 * =============================================================================
 * HOMEWORK 1 SOLUTION: Task Scheduler with Priority Queue
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTS AND MACROS
 * =============================================================================
 */

#define INITIAL_CAPACITY 16
#define MAX_DESCRIPTION 64

#define PARENT(i)       (((i) - 1) / 2)
#define LEFT_CHILD(i)   (2 * (i) + 1)
#define RIGHT_CHILD(i)  (2 * (i) + 2)

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * Task structure
 */
typedef struct {
    int id;                         /* Unique task identifier */
    int priority;                   /* Priority 1-10 (10 = highest) */
    time_t arrival_time;            /* When task was added */
    int execution_time_ms;          /* Estimated execution time */
    char description[MAX_DESCRIPTION];
} Task;

/**
 * Scheduling policy type
 */
typedef enum {
    PRIORITY_FIRST,                 /* Pure priority ordering */
    PRIORITY_WITH_AGING             /* Priority + aging to prevent starvation */
} SchedulingPolicy;

/**
 * Comparator function type
 */
typedef int (*TaskComparator)(const Task *a, const Task *b, time_t current_time);

/**
 * Statistics tracking
 */
typedef struct {
    unsigned long total_tasks;
    double total_wait_time;
    double max_wait_time;
    time_t start_time;
} SchedulerStats;

/**
 * Scheduler structure
 */
typedef struct {
    Task *tasks;                    /* Heap array */
    size_t size;
    size_t capacity;
    SchedulingPolicy policy;
    TaskComparator compare;
    SchedulerStats stats;
} Scheduler;

/* =============================================================================
 * COMPARATOR FUNCTIONS
 * =============================================================================
 */

/**
 * Priority-first comparator
 * Higher priority = higher value in max-heap
 */
int compare_priority_first(const Task *a, const Task *b, time_t current_time) {
    (void)current_time;  /* Unused */
    
    if (a->priority != b->priority)
        return a->priority - b->priority;
    
    /* Tie-breaker: earlier arrival time gets priority */
    return (int)(b->arrival_time - a->arrival_time);
}

/**
 * Priority with aging comparator
 * Effective priority = base priority + (wait_time / 60) [1 point per minute]
 */
int compare_priority_aging(const Task *a, const Task *b, time_t current_time) {
    double wait_a = difftime(current_time, a->arrival_time);
    double wait_b = difftime(current_time, b->arrival_time);
    
    /* Effective priority with aging bonus */
    double eff_a = a->priority + (wait_a / 60.0);
    double eff_b = b->priority + (wait_b / 60.0);
    
    if (eff_a > eff_b) return 1;
    if (eff_a < eff_b) return -1;
    return 0;
}

/* =============================================================================
 * SCHEDULER OPERATIONS
 * =============================================================================
 */

/**
 * Create a new scheduler
 */
Scheduler *scheduler_create(size_t capacity, SchedulingPolicy policy) {
    Scheduler *s = malloc(sizeof(Scheduler));
    if (!s) return NULL;
    
    s->tasks = malloc(capacity * sizeof(Task));
    if (!s->tasks) {
        free(s);
        return NULL;
    }
    
    s->size = 0;
    s->capacity = capacity;
    s->policy = policy;
    
    /* Set comparator based on policy */
    switch (policy) {
        case PRIORITY_WITH_AGING:
            s->compare = compare_priority_aging;
            break;
        case PRIORITY_FIRST:
        default:
            s->compare = compare_priority_first;
            break;
    }
    
    /* Initialise statistics */
    s->stats.total_tasks = 0;
    s->stats.total_wait_time = 0;
    s->stats.max_wait_time = 0;
    s->stats.start_time = time(NULL);
    
    return s;
}

/**
 * Destroy scheduler
 */
void scheduler_destroy(Scheduler *s) {
    if (s) {
        free(s->tasks);
        free(s);
    }
}

/**
 * Sift up for max-heap
 */
static void sift_up(Scheduler *s, size_t i) {
    time_t now = time(NULL);
    
    while (i > 0) {
        size_t parent = PARENT(i);
        
        if (s->compare(&s->tasks[i], &s->tasks[parent], now) <= 0)
            break;
        
        /* Swap */
        Task temp = s->tasks[i];
        s->tasks[i] = s->tasks[parent];
        s->tasks[parent] = temp;
        
        i = parent;
    }
}

/**
 * Sift down for max-heap
 */
static void sift_down(Scheduler *s, size_t i) {
    time_t now = time(NULL);
    
    while (true) {
        size_t largest = i;
        size_t left = LEFT_CHILD(i);
        size_t right = RIGHT_CHILD(i);
        
        if (left < s->size && 
            s->compare(&s->tasks[left], &s->tasks[largest], now) > 0)
            largest = left;
        
        if (right < s->size && 
            s->compare(&s->tasks[right], &s->tasks[largest], now) > 0)
            largest = right;
        
        if (largest == i)
            break;
        
        Task temp = s->tasks[i];
        s->tasks[i] = s->tasks[largest];
        s->tasks[largest] = temp;
        
        i = largest;
    }
}

/**
 * Add task to scheduler
 */
bool scheduler_add_task(Scheduler *s, const Task *task) {
    if (!s || !task) return false;
    
    /* Resize if needed */
    if (s->size >= s->capacity) {
        size_t new_capacity = s->capacity * 2;
        Task *new_tasks = realloc(s->tasks, new_capacity * sizeof(Task));
        if (!new_tasks) return false;
        
        s->tasks = new_tasks;
        s->capacity = new_capacity;
    }
    
    /* Copy task */
    s->tasks[s->size] = *task;
    s->size++;
    
    /* Restore heap property */
    sift_up(s, s->size - 1);
    
    return true;
}

/**
 * Get next task (extract highest priority)
 */
bool scheduler_get_next(Scheduler *s, Task *task) {
    if (!s || s->size == 0 || !task) return false;
    
    time_t now = time(NULL);
    
    /* Calculate wait time for statistics */
    double wait_time = difftime(now, s->tasks[0].arrival_time);
    s->stats.total_wait_time += wait_time;
    if (wait_time > s->stats.max_wait_time)
        s->stats.max_wait_time = wait_time;
    s->stats.total_tasks++;
    
    /* Copy task */
    *task = s->tasks[0];
    
    /* Move last to root */
    s->tasks[0] = s->tasks[s->size - 1];
    s->size--;
    
    /* Restore heap property */
    if (s->size > 0)
        sift_down(s, 0);
    
    return true;
}

/**
 * Peek at next task without removal
 */
bool scheduler_peek_next(const Scheduler *s, Task *task) {
    if (!s || s->size == 0 || !task) return false;
    
    *task = s->tasks[0];
    return true;
}

/**
 * Get scheduler size
 */
size_t scheduler_size(const Scheduler *s) {
    return s ? s->size : 0;
}

/**
 * Check if scheduler is empty
 */
bool scheduler_is_empty(const Scheduler *s) {
    return s == NULL || s->size == 0;
}

/**
 * Print statistics
 */
void scheduler_print_stats(const Scheduler *s) {
    if (!s) return;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Scheduler Statistics                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    time_t now = time(NULL);
    double runtime = difftime(now, s->stats.start_time);
    
    printf("  Total tasks processed:  %lu\n", s->stats.total_tasks);
    printf("  Tasks remaining:        %zu\n", s->size);
    
    if (s->stats.total_tasks > 0) {
        printf("  Average wait time:      %.2f seconds\n", 
               s->stats.total_wait_time / s->stats.total_tasks);
        printf("  Maximum wait time:      %.2f seconds\n", 
               s->stats.max_wait_time);
    }
    
    if (runtime > 0) {
        printf("  Throughput:             %.2f tasks/second\n", 
               s->stats.total_tasks / runtime);
    }
    
    printf("  Scheduling policy:      %s\n",
           s->policy == PRIORITY_FIRST ? "Priority First" : "Priority with Aging");
    printf("\n");
}

/* =============================================================================
 * DEMONSTRATION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     HOMEWORK 1: Task Scheduler with Priority Queue            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Test with Priority First policy */
    printf("=== Testing Priority First Policy ===\n\n");
    
    Scheduler *s = scheduler_create(INITIAL_CAPACITY, PRIORITY_FIRST);
    if (!s) {
        fprintf(stderr, "Failed to create scheduler\n");
        return 1;
    }
    
    /* Create test tasks */
    Task tasks[] = {
        {1001, 5, time(NULL), 100, "Database backup"},
        {1002, 8, time(NULL), 50,  "User authentication"},
        {1003, 3, time(NULL), 200, "Log rotation"},
        {1004, 10, time(NULL), 25, "Security alert"},
        {1005, 7, time(NULL), 75,  "API request"},
        {1006, 2, time(NULL), 150, "Cache cleanup"}
    };
    size_t num_tasks = sizeof(tasks) / sizeof(tasks[0]);
    
    /* Add tasks */
    printf("Adding tasks:\n");
    for (size_t i = 0; i < num_tasks; i++) {
        printf("  + [Priority %2d] Task %d: %s\n", 
               tasks[i].priority, tasks[i].id, tasks[i].description);
        scheduler_add_task(s, &tasks[i]);
    }
    printf("\n");
    
    /* Process tasks */
    printf("Processing tasks (highest priority first):\n");
    Task current;
    int order = 1;
    while (scheduler_get_next(s, &current)) {
        printf("  %d. [Priority %2d] Task %d: %s\n",
               order++, current.priority, current.id, current.description);
    }
    
    scheduler_print_stats(s);
    scheduler_destroy(s);
    
    /* Test with aging policy */
    printf("\n=== Testing Priority with Aging Policy ===\n\n");
    
    s = scheduler_create(INITIAL_CAPACITY, PRIORITY_WITH_AGING);
    
    /* Re-add tasks with varied arrival times (simulate aging) */
    for (size_t i = 0; i < num_tasks; i++) {
        tasks[i].arrival_time = time(NULL) - (i * 120);  /* Older tasks */
        scheduler_add_task(s, &tasks[i]);
    }
    
    printf("With aging, older low-priority tasks gain effective priority.\n");
    printf("Task 1006 (priority 2, oldest) may now compete with newer high-priority tasks.\n\n");
    
    printf("Processing with aging:\n");
    order = 1;
    while (scheduler_get_next(s, &current)) {
        printf("  %d. [Base Priority %2d] Task %d: %s\n",
               order++, current.priority, current.id, current.description);
    }
    
    scheduler_print_stats(s);
    scheduler_destroy(s);
    
    printf("✓ Homework 1 solution demonstration complete.\n\n");
    
    return 0;
}
