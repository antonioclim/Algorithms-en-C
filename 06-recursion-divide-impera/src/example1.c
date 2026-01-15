/**
 * =============================================================================
 * WEEK 06: QUEUES
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Basic queue structure and operations
 *   2. Circular buffer implementation
 *   3. Linked list queue implementation
 *   4. Queue visualisation techniques
 *   5. BFS graph traversal using queues
 *   6. Round-robin process scheduling simulation
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Usage: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * PART 1: ARRAY-BASED CIRCULAR QUEUE
 * =============================================================================
 */

#define ARRAY_QUEUE_CAPACITY 8

/**
 * Circular queue structure using a fixed-size array.
 * Uses a count field to distinguish empty from full states.
 */
typedef struct {
    int data[ARRAY_QUEUE_CAPACITY];
    int front;      /* Index of the front element */
    int rear;       /* Index of the next insertion point */
    int count;      /* Number of elements currently in queue */
} ArrayQueue;

/**
 * Initialise an array-based queue.
 */
void aq_init(ArrayQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

/**
 * Check if the queue is empty.
 */
bool aq_is_empty(const ArrayQueue *q) {
    return q->count == 0;
}

/**
 * Check if the queue is full.
 */
bool aq_is_full(const ArrayQueue *q) {
    return q->count == ARRAY_QUEUE_CAPACITY;
}

/**
 * Get the current number of elements.
 */
int aq_size(const ArrayQueue *q) {
    return q->count;
}

/**
 * Add an element to the rear of the queue.
 * Returns true on success, false if queue is full.
 */
bool aq_enqueue(ArrayQueue *q, int item) {
    if (aq_is_full(q)) {
        fprintf(stderr, "  [ERROR] Queue overflow - cannot enqueue %d\n", item);
        return false;
    }
    
    q->data[q->rear] = item;
    q->rear = (q->rear + 1) % ARRAY_QUEUE_CAPACITY;  /* Circular wraparound */
    q->count++;
    
    return true;
}

/**
 * Remove and return the front element.
 * Returns true on success, false if queue is empty.
 */
bool aq_dequeue(ArrayQueue *q, int *item) {
    if (aq_is_empty(q)) {
        fprintf(stderr, "  [ERROR] Queue underflow - cannot dequeue\n");
        return false;
    }
    
    *item = q->data[q->front];
    q->front = (q->front + 1) % ARRAY_QUEUE_CAPACITY;
    q->count--;
    
    return true;
}

/**
 * View the front element without removing it.
 */
bool aq_peek(const ArrayQueue *q, int *item) {
    if (aq_is_empty(q)) {
        return false;
    }
    *item = q->data[q->front];
    return true;
}

/**
 * Display the queue contents with visual representation.
 */
void aq_display(const ArrayQueue *q) {
    printf("  Array Queue State:\n");
    printf("  ┌");
    for (int i = 0; i < ARRAY_QUEUE_CAPACITY; i++) {
        printf("─────");
        if (i < ARRAY_QUEUE_CAPACITY - 1) printf("┬");
    }
    printf("┐\n  │");
    
    /* Print array contents */
    for (int i = 0; i < ARRAY_QUEUE_CAPACITY; i++) {
        int idx = (q->front + i) % ARRAY_QUEUE_CAPACITY;
        if (i < q->count) {
            printf(" %3d │", q->data[idx]);
        } else {
            printf("  -  │");
        }
    }
    
    printf("\n  └");
    for (int i = 0; i < ARRAY_QUEUE_CAPACITY; i++) {
        printf("─────");
        if (i < ARRAY_QUEUE_CAPACITY - 1) printf("┴");
    }
    printf("┘\n");
    
    printf("  front=%d, rear=%d, count=%d\n", q->front, q->rear, q->count);
}

/**
 * Demonstrate array-based queue operations.
 */
void demo_array_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: ARRAY-BASED CIRCULAR QUEUE                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ArrayQueue q;
    aq_init(&q);
    
    printf("  Initial state (empty queue):\n");
    aq_display(&q);
    
    /* Enqueue several elements */
    printf("\n  Enqueuing elements: 10, 20, 30, 40, 50\n");
    aq_enqueue(&q, 10);
    aq_enqueue(&q, 20);
    aq_enqueue(&q, 30);
    aq_enqueue(&q, 40);
    aq_enqueue(&q, 50);
    aq_display(&q);
    
    /* Dequeue some elements */
    int item;
    printf("\n  Dequeuing 3 elements:\n");
    for (int i = 0; i < 3; i++) {
        if (aq_dequeue(&q, &item)) {
            printf("    Dequeued: %d\n", item);
        }
    }
    aq_display(&q);
    
    /* Enqueue more to demonstrate wraparound */
    printf("\n  Enqueuing more elements (demonstrating wraparound): 60, 70, 80, 90\n");
    aq_enqueue(&q, 60);
    aq_enqueue(&q, 70);
    aq_enqueue(&q, 80);
    aq_enqueue(&q, 90);
    aq_display(&q);
    
    /* Peek at front */
    if (aq_peek(&q, &item)) {
        printf("\n  Peek at front element: %d\n", item);
    }
    
    /* Try to overflow */
    printf("\n  Attempting to enqueue when full:\n");
    aq_enqueue(&q, 100);
    aq_enqueue(&q, 110);
    aq_enqueue(&q, 120);
}

/* =============================================================================
 * PART 2: LINKED LIST QUEUE
 * =============================================================================
 */

/**
 * Node structure for linked list queue.
 */
typedef struct LQNode {
    int data;
    struct LQNode *next;
} LQNode;

/**
 * Linked list queue structure.
 */
typedef struct {
    LQNode *front;  /* Points to first node (for dequeue) */
    LQNode *rear;   /* Points to last node (for enqueue) */
    int count;
} LinkedQueue;

/**
 * Initialise a linked list queue.
 */
void lq_init(LinkedQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * Check if the linked queue is empty.
 */
bool lq_is_empty(const LinkedQueue *q) {
    return q->front == NULL;
}

/**
 * Get the size of the linked queue.
 */
int lq_size(const LinkedQueue *q) {
    return q->count;
}

/**
 * Enqueue an element (add to rear).
 */
bool lq_enqueue(LinkedQueue *q, int item) {
    LQNode *new_node = (LQNode *)malloc(sizeof(LQNode));
    if (!new_node) {
        fprintf(stderr, "  [ERROR] Memory allocation failed\n");
        return false;
    }
    
    new_node->data = item;
    new_node->next = NULL;
    
    if (q->rear == NULL) {
        /* Queue was empty - new node is both front and rear */
        q->front = new_node;
        q->rear = new_node;
    } else {
        /* Append to rear */
        q->rear->next = new_node;
        q->rear = new_node;
    }
    
    q->count++;
    return true;
}

/**
 * Dequeue an element (remove from front).
 */
bool lq_dequeue(LinkedQueue *q, int *item) {
    if (lq_is_empty(q)) {
        fprintf(stderr, "  [ERROR] Queue underflow\n");
        return false;
    }
    
    LQNode *temp = q->front;
    *item = temp->data;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        /* Queue is now empty */
        q->rear = NULL;
    }
    
    free(temp);
    q->count--;
    return true;
}

/**
 * Peek at the front element.
 */
bool lq_peek(const LinkedQueue *q, int *item) {
    if (lq_is_empty(q)) {
        return false;
    }
    *item = q->front->data;
    return true;
}

/**
 * Display the linked queue.
 */
void lq_display(const LinkedQueue *q) {
    printf("  Linked Queue: front -> ");
    
    LQNode *current = q->front;
    while (current != NULL) {
        printf("[%d]", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    
    printf(" <- rear\n");
    printf("  Size: %d\n", q->count);
}

/**
 * Free all nodes in the linked queue.
 */
void lq_destroy(LinkedQueue *q) {
    LQNode *current = q->front;
    while (current != NULL) {
        LQNode *temp = current;
        current = current->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * Demonstrate linked list queue operations.
 */
void demo_linked_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: LINKED LIST QUEUE                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    LinkedQueue q;
    lq_init(&q);
    
    printf("  Enqueuing elements: A(65), B(66), C(67), D(68), E(69)\n");
    lq_enqueue(&q, 65);
    lq_enqueue(&q, 66);
    lq_enqueue(&q, 67);
    lq_enqueue(&q, 68);
    lq_enqueue(&q, 69);
    lq_display(&q);
    
    int item;
    printf("\n  Dequeuing 2 elements:\n");
    for (int i = 0; i < 2; i++) {
        if (lq_dequeue(&q, &item)) {
            printf("    Dequeued: %d ('%c')\n", item, item);
        }
    }
    lq_display(&q);
    
    printf("\n  Enqueuing more: F(70), G(71)\n");
    lq_enqueue(&q, 70);
    lq_enqueue(&q, 71);
    lq_display(&q);
    
    /* Clean up */
    lq_destroy(&q);
    printf("\n  Queue destroyed (memory freed)\n");
}

/* =============================================================================
 * PART 3: DYNAMIC CAPACITY QUEUE
 * =============================================================================
 */

/**
 * Queue with dynamic memory allocation and resize capability.
 */
typedef struct {
    int *data;
    int front;
    int rear;
    int count;
    int capacity;
} DynamicQueue;

/**
 * Create a dynamic queue with initial capacity.
 */
DynamicQueue *dq_create(int initial_capacity) {
    DynamicQueue *q = (DynamicQueue *)malloc(sizeof(DynamicQueue));
    if (!q) return NULL;
    
    q->data = (int *)malloc(sizeof(int) * initial_capacity);
    if (!q->data) {
        free(q);
        return NULL;
    }
    
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->capacity = initial_capacity;
    
    return q;
}

/**
 * Double the queue capacity when full.
 */
bool dq_resize(DynamicQueue *q) {
    int new_capacity = q->capacity * 2;
    int *new_data = (int *)malloc(sizeof(int) * new_capacity);
    if (!new_data) return false;
    
    /* Copy elements in order */
    for (int i = 0; i < q->count; i++) {
        int idx = (q->front + i) % q->capacity;
        new_data[i] = q->data[idx];
    }
    
    free(q->data);
    q->data = new_data;
    q->front = 0;
    q->rear = q->count;
    q->capacity = new_capacity;
    
    return true;
}

/**
 * Enqueue with automatic resize.
 */
bool dq_enqueue(DynamicQueue *q, int item) {
    if (q->count >= q->capacity) {
        printf("    [INFO] Resizing queue from %d to %d\n", q->capacity, q->capacity * 2);
        if (!dq_resize(q)) {
            return false;
        }
    }
    
    q->data[q->rear] = item;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    return true;
}

/**
 * Dequeue from dynamic queue.
 */
bool dq_dequeue(DynamicQueue *q, int *item) {
    if (q->count == 0) return false;
    
    *item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return true;
}

/**
 * Destroy dynamic queue.
 */
void dq_destroy(DynamicQueue *q) {
    if (q) {
        free(q->data);
        free(q);
    }
}

/**
 * Demonstrate dynamic queue with auto-resize.
 */
void demo_dynamic_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: DYNAMIC QUEUE WITH AUTO-RESIZE                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    DynamicQueue *q = dq_create(4);  /* Start with small capacity */
    
    printf("  Initial capacity: %d\n", q->capacity);
    printf("  Enqueuing 10 elements to trigger resize:\n\n");
    
    for (int i = 1; i <= 10; i++) {
        printf("    Enqueue(%d) - count: %d, capacity: %d\n", i * 10, q->count + 1, q->capacity);
        dq_enqueue(q, i * 10);
    }
    
    printf("\n  Final state: count=%d, capacity=%d\n", q->count, q->capacity);
    
    printf("\n  Dequeuing all elements:\n    ");
    int item;
    while (dq_dequeue(q, &item)) {
        printf("%d ", item);
    }
    printf("\n");
    
    dq_destroy(q);
}

/* =============================================================================
 * PART 4: BFS GRAPH TRAVERSAL
 * =============================================================================
 */

#define MAX_VERTICES 10

/**
 * Perform BFS traversal on an adjacency matrix graph.
 */
void bfs_traversal(int graph[MAX_VERTICES][MAX_VERTICES], int vertices, int start) {
    bool visited[MAX_VERTICES] = {false};
    ArrayQueue q;
    aq_init(&q);
    
    visited[start] = true;
    aq_enqueue(&q, start);
    
    printf("  BFS traversal starting from vertex %d:\n    ", start);
    
    while (!aq_is_empty(&q)) {
        int current;
        aq_dequeue(&q, &current);
        printf("%d ", current);
        
        /* Visit all unvisited neighbours */
        for (int i = 0; i < vertices; i++) {
            if (graph[current][i] && !visited[i]) {
                visited[i] = true;
                aq_enqueue(&q, i);
            }
        }
    }
    printf("\n");
}

/**
 * Demonstrate BFS using queue.
 */
void demo_bfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: BFS GRAPH TRAVERSAL                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /*
     * Graph structure:
     *      0 --- 1 --- 2
     *      |     |     |
     *      3 --- 4 --- 5
     */
    int graph[MAX_VERTICES][MAX_VERTICES] = {0};
    
    /* Define edges */
    graph[0][1] = graph[1][0] = 1;  /* 0-1 */
    graph[1][2] = graph[2][1] = 1;  /* 1-2 */
    graph[0][3] = graph[3][0] = 1;  /* 0-3 */
    graph[1][4] = graph[4][1] = 1;  /* 1-4 */
    graph[2][5] = graph[5][2] = 1;  /* 2-5 */
    graph[3][4] = graph[4][3] = 1;  /* 3-4 */
    graph[4][5] = graph[5][4] = 1;  /* 4-5 */
    
    printf("  Graph structure:\n");
    printf("       0 --- 1 --- 2\n");
    printf("       |     |     |\n");
    printf("       3 --- 4 --- 5\n\n");
    
    bfs_traversal(graph, 6, 0);
    printf("\n");
    bfs_traversal(graph, 6, 3);
}

/* =============================================================================
 * PART 5: ROUND-ROBIN SCHEDULING
 * =============================================================================
 */

/**
 * Process structure for scheduling simulation.
 */
typedef struct {
    int pid;
    char name[32];
    int burst_time;
    int remaining_time;
    int arrival_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
} Process;

/**
 * Simulate round-robin CPU scheduling.
 */
void round_robin_scheduler(Process processes[], int n, int quantum) {
    ArrayQueue ready_queue;
    aq_init(&ready_queue);
    
    /* Load all processes into ready queue */
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        aq_enqueue(&ready_queue, i);
    }
    
    int current_time = 0;
    int completed = 0;
    
    printf("  Time Quantum: %d ms\n\n", quantum);
    printf("  Execution Timeline:\n");
    printf("  ┌──────────────────────────────────────────────────────────┐\n");
    
    while (!aq_is_empty(&ready_queue)) {
        int idx;
        aq_dequeue(&ready_queue, &idx);
        
        Process *p = &processes[idx];
        int run_time = (p->remaining_time < quantum) ? p->remaining_time : quantum;
        
        printf("  │ [%3d-%3d] %s (PID %d) ", current_time, current_time + run_time, 
               p->name, p->pid);
        
        /* Print progress bar */
        int bar_width = 20;
        int progress = (int)((float)(p->burst_time - p->remaining_time + run_time) / 
                             p->burst_time * bar_width);
        printf("[");
        for (int i = 0; i < bar_width; i++) {
            printf(i < progress ? "█" : "░");
        }
        printf("]");
        
        current_time += run_time;
        p->remaining_time -= run_time;
        
        if (p->remaining_time > 0) {
            printf(" (remaining: %d ms)\n", p->remaining_time);
            aq_enqueue(&ready_queue, idx);  /* Re-queue if not finished */
        } else {
            printf(" ✓ DONE\n");
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            completed++;
        }
    }
    
    printf("  └──────────────────────────────────────────────────────────┘\n");
    
    /* Calculate and display statistics */
    printf("\n  Process Statistics:\n");
    printf("  ┌─────────────┬───────────┬────────────┬────────────┬───────────┐\n");
    printf("  │ Process     │ Burst(ms) │ Finish(ms) │ TAT(ms)    │ Wait(ms)  │\n");
    printf("  ├─────────────┼───────────┼────────────┼────────────┼───────────┤\n");
    
    float avg_tat = 0, avg_wait = 0;
    for (int i = 0; i < n; i++) {
        printf("  │ %-11s │    %3d    │    %3d     │    %3d     │    %3d    │\n",
               processes[i].name, processes[i].burst_time,
               processes[i].completion_time, processes[i].turnaround_time,
               processes[i].waiting_time);
        avg_tat += processes[i].turnaround_time;
        avg_wait += processes[i].waiting_time;
    }
    
    printf("  └─────────────┴───────────┴────────────┴────────────┴───────────┘\n");
    printf("\n  Average Turnaround Time: %.2f ms\n", avg_tat / n);
    printf("  Average Waiting Time:    %.2f ms\n", avg_wait / n);
}

/**
 * Demonstrate round-robin scheduling.
 */
void demo_scheduling(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: ROUND-ROBIN CPU SCHEDULING                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Process processes[] = {
        {1, "Chrome", 10, 0, 0, 0, 0, 0},
        {2, "VSCode", 6, 0, 0, 0, 0, 0},
        {3, "Spotify", 8, 0, 0, 0, 0, 0},
        {4, "Terminal", 4, 0, 0, 0, 0, 0}
    };
    int n = sizeof(processes) / sizeof(processes[0]);
    
    round_robin_scheduler(processes, n, 3);
}

/* =============================================================================
 * PART 6: QUEUE PERFORMANCE COMPARISON
 * =============================================================================
 */

#define PERF_OPERATIONS 100000

/**
 * Compare performance of different queue implementations.
 */
void demo_performance(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: PERFORMANCE COMPARISON                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Performing %d enqueue/dequeue operations...\n\n", PERF_OPERATIONS);
    
    clock_t start, end;
    double cpu_time;
    
    /* Array-based queue (using dynamic for larger capacity) */
    printf("  Array-based circular queue:\n");
    DynamicQueue *aq = dq_create(PERF_OPERATIONS);
    
    start = clock();
    for (int i = 0; i < PERF_OPERATIONS; i++) {
        dq_enqueue(aq, i);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("    Enqueue: %.3f ms\n", cpu_time);
    
    start = clock();
    int item;
    for (int i = 0; i < PERF_OPERATIONS; i++) {
        dq_dequeue(aq, &item);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("    Dequeue: %.3f ms\n", cpu_time);
    
    dq_destroy(aq);
    
    /* Linked list queue */
    printf("\n  Linked list queue:\n");
    LinkedQueue lq;
    lq_init(&lq);
    
    start = clock();
    for (int i = 0; i < PERF_OPERATIONS; i++) {
        lq_enqueue(&lq, i);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("    Enqueue: %.3f ms\n", cpu_time);
    
    start = clock();
    for (int i = 0; i < PERF_OPERATIONS; i++) {
        lq_dequeue(&lq, &item);
    }
    end = clock();
    cpu_time = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
    printf("    Dequeue: %.3f ms\n", cpu_time);
    
    lq_destroy(&lq);
    
    printf("\n  Note: Array-based queues typically have better cache locality,\n");
    printf("        whilst linked lists avoid the need for contiguous memory.\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     WEEK 06: QUEUES - Complete Example                        ║\n");
    printf("║     ATP - Algorithms and Programming Techniques               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_array_queue();
    demo_linked_queue();
    demo_dynamic_queue();
    demo_bfs();
    demo_scheduling();
    demo_performance();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     DEMONSTRATION COMPLETE                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
