/**
 * =============================================================================
 * EXERCISE 1: CIRCULAR BUFFER QUEUE
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement a complete circular buffer queue that handles integer values,
 *   supports interactive commands and provides visual feedback of queue state.
 *
 * REQUIREMENTS:
 *   1. Implement the CircularQueue structure with dynamic allocation
 *   2. Create all core queue operations (init, enqueue, dequeue, peek)
 *   3. Handle overflow and underflow conditions gracefully
 *   4. Implement a visual display function showing the circular nature
 *   5. Process commands from standard input
 *   6. Track and report statistics (total enqueues, dequeues, overflow/underflow counts)
 *
 * SUPPORTED COMMANDS:
 *   ENQUEUE <value>  - Add value to queue
 *   DEQUEUE          - Remove and display front element
 *   PEEK             - Display front element without removal
 *   SIZE             - Display current element count
 *   DISPLAY          - Show visual representation of queue
 *   STATS            - Show operation statistics
 *   QUIT             - Exit program
 *
 * EXAMPLE INPUT:
 *   ENQUEUE 10
 *   ENQUEUE 20
 *   ENQUEUE 30
 *   DISPLAY
 *   DEQUEUE
 *   PEEK
 *   SIZE
 *   QUIT
 *
 * EXPECTED OUTPUT:
 *   > ENQUEUE 10
 *   Enqueued: 10
 *   > ENQUEUE 20
 *   Enqueued: 20
 *   > ENQUEUE 30
 *   Enqueued: 30
 *   > DISPLAY
 *   Queue: [10] [20] [30] [ ] [ ] [ ] [ ] [ ]
 *          ^front          ^rear
 *   > DEQUEUE
 *   Dequeued: 10
 *   > PEEK
 *   Front element: 20
 *   > SIZE
 *   Queue size: 2
 *   > QUIT
 *   Goodbye!
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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

#define DEFAULT_CAPACITY 8
#define MAX_COMMAND_LENGTH 64

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define the CircularQueue structure
 *
 * The structure should contain:
 *   - data: pointer to dynamically allocated integer array
 *   - front: index of the front element
 *   - rear: index of the next insertion point
 *   - count: current number of elements
 *   - capacity: maximum number of elements
 *
 * Hint: Use 'int *data' for the dynamic array
 */
typedef struct {
    /* YOUR CODE HERE */
    int placeholder;  /* Remove this line when you add your fields */
} CircularQueue;

/**
 * Statistics structure to track operations.
 */
typedef struct {
    int total_enqueues;
    int total_dequeues;
    int overflow_count;
    int underflow_count;
} QueueStats;

/* =============================================================================
 * FUNCTION PROTOTYPES
 * =============================================================================
 */

CircularQueue *cq_create(int capacity);
void cq_destroy(CircularQueue *q);
bool cq_is_empty(const CircularQueue *q);
bool cq_is_full(const CircularQueue *q);
int cq_size(const CircularQueue *q);
bool cq_enqueue(CircularQueue *q, int item, QueueStats *stats);
bool cq_dequeue(CircularQueue *q, int *item, QueueStats *stats);
bool cq_peek(const CircularQueue *q, int *item);
void cq_display(const CircularQueue *q);
void cq_display_circular(const CircularQueue *q);
void print_stats(const QueueStats *stats);
void process_commands(CircularQueue *q, QueueStats *stats);

/* =============================================================================
 * QUEUE CREATION AND DESTRUCTION
 * =============================================================================
 */

/**
 * TODO 2: Implement queue creation
 *
 * @param capacity  Maximum number of elements the queue can hold
 * @return          Pointer to newly created queue, or NULL on failure
 *
 * Steps:
 *   1. Allocate memory for the CircularQueue structure
 *   2. Check if allocation succeeded; return NULL if not
 *   3. Allocate memory for the data array (capacity integers)
 *   4. Check if allocation succeeded; free structure and return NULL if not
 *   5. Initialise front, rear and count to 0
 *   6. Set capacity to the given value
 *   7. Return pointer to the queue
 */
CircularQueue *cq_create(int capacity) {
    /* YOUR CODE HERE */
    return NULL;  /* Replace this */
}

/**
 * TODO 3: Implement queue destruction
 *
 * @param q  Pointer to queue to destroy
 *
 * Steps:
 *   1. Check if q is NULL; return immediately if so
 *   2. Free the data array
 *   3. Free the queue structure
 *
 * Hint: Always check for NULL before freeing
 */
void cq_destroy(CircularQueue *q) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * QUEUE STATE FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 4: Implement isEmpty check
 *
 * @param q  Pointer to queue
 * @return   true if queue is empty, false otherwise
 *
 * Hint: The queue is empty when count equals 0
 */
bool cq_is_empty(const CircularQueue *q) {
    /* YOUR CODE HERE */
    return true;  /* Replace this */
}

/**
 * TODO 5: Implement isFull check
 *
 * @param q  Pointer to queue
 * @return   true if queue is full, false otherwise
 *
 * Hint: The queue is full when count equals capacity
 */
bool cq_is_full(const CircularQueue *q) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/**
 * Return the current number of elements in the queue.
 */
int cq_size(const CircularQueue *q) {
    if (!q) return 0;
    return 0;  /* TODO: Return the actual count */
}

/* =============================================================================
 * CORE QUEUE OPERATIONS
 * =============================================================================
 */

/**
 * TODO 6: Implement enqueue operation
 *
 * @param q      Pointer to queue
 * @param item   Value to insert
 * @param stats  Pointer to statistics structure (update on success/overflow)
 * @return       true on success, false if queue is full
 *
 * Steps:
 *   1. Check if queue is full
 *   2. If full, increment stats->overflow_count and return false
 *   3. Store item at data[rear]
 *   4. Update rear using circular increment: (rear + 1) % capacity
 *   5. Increment count
 *   6. Increment stats->total_enqueues
 *   7. Return true
 *
 * Hint: The modulo operator (%) handles the circular wraparound
 */
bool cq_enqueue(CircularQueue *q, int item, QueueStats *stats) {
    if (!q) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 7: Implement dequeue operation
 *
 * @param q      Pointer to queue
 * @param item   Pointer to store the removed value
 * @param stats  Pointer to statistics structure (update on success/underflow)
 * @return       true on success, false if queue is empty
 *
 * Steps:
 *   1. Check if queue is empty
 *   2. If empty, increment stats->underflow_count and return false
 *   3. Store data[front] in *item
 *   4. Update front using circular increment: (front + 1) % capacity
 *   5. Decrement count
 *   6. Increment stats->total_dequeues
 *   7. Return true
 */
bool cq_dequeue(CircularQueue *q, int *item, QueueStats *stats) {
    if (!q || !item) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 8: Implement peek operation
 *
 * @param q     Pointer to queue
 * @param item  Pointer to store the front value (without removal)
 * @return      true on success, false if queue is empty
 *
 * Steps:
 *   1. Check if queue is empty; return false if so
 *   2. Store data[front] in *item
 *   3. Return true (do NOT modify front or count)
 */
bool cq_peek(const CircularQueue *q, int *item) {
    if (!q || !item) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/* =============================================================================
 * DISPLAY FUNCTIONS
 * =============================================================================
 */

/**
 * Display queue contents in linear format.
 */
void cq_display(const CircularQueue *q) {
    if (!q) {
        printf("Queue is NULL\n");
        return;
    }
    
    printf("Queue: ");
    
    /* Display each slot */
    for (int i = 0; i < q->capacity; i++) {
        /* Calculate actual index considering circular nature */
        int idx = (q->front + i) % q->capacity;
        
        if (i < q->count) {
            printf("[%3d] ", q->data[idx]);
        } else {
            printf("[   ] ");
        }
    }
    printf("\n");
    
    /* Display front and rear markers */
    printf("       ");
    for (int i = 0; i < q->capacity; i++) {
        if (i == 0) {
            printf("^front");
        } else if (i == q->count) {
            printf("^rear ");
        } else {
            printf("      ");
        }
    }
    printf("\n");
}

/**
 * Display queue as a circular buffer visualisation.
 */
void cq_display_circular(const CircularQueue *q) {
    if (!q) return;
    
    printf("\n  Circular Buffer Visualisation:\n");
    printf("  Capacity: %d, Count: %d\n", q->capacity, q->count);
    printf("  Front index: %d, Rear index: %d\n\n", q->front, q->rear);
    
    /* Simple circular representation */
    printf("     ");
    for (int i = 0; i < q->capacity; i++) {
        printf("┌─────┐");
    }
    printf("\n     ");
    
    for (int i = 0; i < q->capacity; i++) {
        bool has_data = false;
        int value = 0;
        
        /* Check if this slot contains data */
        for (int j = 0; j < q->count; j++) {
            if ((q->front + j) % q->capacity == i) {
                has_data = true;
                value = q->data[i];
                break;
            }
        }
        
        if (has_data) {
            printf("│%4d │", value);
        } else {
            printf("│  -  │");
        }
    }
    printf("\n     ");
    
    for (int i = 0; i < q->capacity; i++) {
        printf("└─────┘");
    }
    printf("\n     ");
    
    /* Index markers */
    for (int i = 0; i < q->capacity; i++) {
        char marker = ' ';
        if (i == q->front && i == q->rear) {
            marker = '*';  /* Both front and rear (empty or full) */
        } else if (i == q->front) {
            marker = 'F';  /* Front */
        } else if (i == q->rear) {
            marker = 'R';  /* Rear */
        }
        printf("  [%d]%c ", i, marker);
    }
    printf("\n");
    printf("  Legend: F=Front, R=Rear, *=Both\n\n");
}

/**
 * Print operation statistics.
 */
void print_stats(const QueueStats *stats) {
    printf("\n  === Queue Statistics ===\n");
    printf("  Total enqueues:    %d\n", stats->total_enqueues);
    printf("  Total dequeues:    %d\n", stats->total_dequeues);
    printf("  Overflow attempts: %d\n", stats->overflow_count);
    printf("  Underflow attempts: %d\n", stats->underflow_count);
    printf("  ==========================\n\n");
}

/* =============================================================================
 * COMMAND PROCESSING
 * =============================================================================
 */

/**
 * Process interactive commands from stdin.
 */
void process_commands(CircularQueue *q, QueueStats *stats) {
    char command[MAX_COMMAND_LENGTH];
    char operation[MAX_COMMAND_LENGTH];
    int value;
    int item;
    
    printf("\nCircular Queue Interactive Mode\n");
    printf("Commands: ENQUEUE <n>, DEQUEUE, PEEK, SIZE, DISPLAY, CIRCULAR, STATS, QUIT\n");
    printf("─────────────────────────────────────────────────────────────────────────\n\n");
    
    while (1) {
        printf("> ");
        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            break;
        }
        
        /* Remove newline */
        command[strcspn(command, "\n")] = 0;
        
        /* Parse command */
        if (sscanf(command, "%s %d", operation, &value) >= 1) {
            
            if (strcmp(operation, "ENQUEUE") == 0) {
                if (sscanf(command, "%*s %d", &value) == 1) {
                    if (cq_enqueue(q, value, stats)) {
                        printf("Enqueued: %d\n", value);
                    } else {
                        printf("Error: Queue is full (overflow)\n");
                    }
                } else {
                    printf("Usage: ENQUEUE <value>\n");
                }
            }
            else if (strcmp(operation, "DEQUEUE") == 0) {
                if (cq_dequeue(q, &item, stats)) {
                    printf("Dequeued: %d\n", item);
                } else {
                    printf("Error: Queue is empty (underflow)\n");
                }
            }
            else if (strcmp(operation, "PEEK") == 0) {
                if (cq_peek(q, &item)) {
                    printf("Front element: %d\n", item);
                } else {
                    printf("Error: Queue is empty\n");
                }
            }
            else if (strcmp(operation, "SIZE") == 0) {
                printf("Queue size: %d / %d\n", cq_size(q), q->capacity);
            }
            else if (strcmp(operation, "DISPLAY") == 0) {
                cq_display(q);
            }
            else if (strcmp(operation, "CIRCULAR") == 0) {
                cq_display_circular(q);
            }
            else if (strcmp(operation, "STATS") == 0) {
                print_stats(stats);
            }
            else if (strcmp(operation, "QUIT") == 0 || strcmp(operation, "EXIT") == 0) {
                printf("Goodbye!\n");
                break;
            }
            else if (strcmp(operation, "HELP") == 0) {
                printf("Available commands:\n");
                printf("  ENQUEUE <n>  - Add value to rear of queue\n");
                printf("  DEQUEUE      - Remove and show front element\n");
                printf("  PEEK         - Show front element without removal\n");
                printf("  SIZE         - Show current queue size\n");
                printf("  DISPLAY      - Show linear queue view\n");
                printf("  CIRCULAR     - Show circular buffer view\n");
                printf("  STATS        - Show operation statistics\n");
                printf("  QUIT         - Exit program\n");
            }
            else {
                printf("Unknown command: %s (type HELP for list)\n", operation);
            }
        }
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCISE 1: CIRCULAR BUFFER QUEUE                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Create queue with default capacity */
    CircularQueue *queue = cq_create(DEFAULT_CAPACITY);
    
    if (!queue) {
        fprintf(stderr, "Error: Failed to create queue\n");
        return 1;
    }
    
    /* Initialise statistics */
    QueueStats stats = {0, 0, 0, 0};
    
    /* Process commands interactively */
    process_commands(queue, &stats);
    
    /* Final statistics */
    printf("\nFinal Statistics:\n");
    print_stats(&stats);
    
    /* Clean up */
    cq_destroy(queue);
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Add a RESIZE command that doubles the queue capacity whilst preserving
 *    existing elements.
 *
 * 2. Implement a REVERSE command that reverses the order of all elements
 *    in the queue.
 *
 * 3. Add SAVE and LOAD commands to persist queue state to a file.
 *
 * 4. Implement a MERGE command that takes another queue's data from file
 *    and merges it with the current queue.
 *
 * 5. Add a SORT command that sorts the queue elements whilst maintaining
 *    the queue structure (front should point to smallest element).
 *
 * =============================================================================
 */
