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
 * NOTE ON MODES:
 *   When standard input is a terminal, the programme runs in interactive mode
 *   and prints prompts and explanatory headers.
 *
 *   When standard input is redirected from a file or pipe, the programme runs
 *   in batch mode and suppresses prompts and banners so that output becomes a
 *   stable artefact suitable for regression testing.
 *
 * COMPILATION:
 *   gcc -Wall -Wextra -std=c11 -o exercise1 src/exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>  /* isatty, fileno */
#endif

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
 * A circular buffer queue storing integers.
 *
 * Representation invariants:
 *   - capacity > 0
 *   - 0 <= front < capacity
 *   - 0 <= rear < capacity
 *   - 0 <= count <= capacity
 *
 * Semantics:
 *   - front points to the oldest element (next to be dequeued)
 *   - rear points to the next insertion slot
 *   - elements occupy exactly the positions (front + i) mod capacity for i in [0, count)
 */
typedef struct {
    int *data;
    int front;
    int rear;
    int count;
    int capacity;
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

static bool stdin_is_tty(void);

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
void process_commands(CircularQueue *q, QueueStats *stats, bool interactive);

/* =============================================================================
 * SMALL ENVIRONMENT HELPERS
 * =============================================================================
 */

static bool stdin_is_tty(void) {
#if defined(__unix__) || defined(__APPLE__)
    return isatty(STDIN_FILENO) != 0;
#else
    /*
     * On non-POSIX platforms isatty may be unavailable.
     * We conservatively assume interactive mode.
     */
    return true;
#endif
}

/* =============================================================================
 * QUEUE CREATION AND DESTRUCTION
 * =============================================================================
 */

CircularQueue *cq_create(int capacity) {
    if (capacity <= 0) {
        capacity = DEFAULT_CAPACITY;
    }

    CircularQueue *q = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (!q) {
        return NULL;
    }

    q->data = (int *)malloc(sizeof(int) * (size_t)capacity);
    if (!q->data) {
        free(q);
        return NULL;
    }

    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->capacity = capacity;

    return q;
}

void cq_destroy(CircularQueue *q) {
    if (!q) {
        return;
    }
    free(q->data);
    q->data = NULL;
    free(q);
}

/* =============================================================================
 * QUEUE STATE FUNCTIONS
 * =============================================================================
 */

bool cq_is_empty(const CircularQueue *q) {
    return (!q) || (q->count == 0);
}

bool cq_is_full(const CircularQueue *q) {
    return (q) && (q->count == q->capacity);
}

int cq_size(const CircularQueue *q) {
    if (!q) {
        return 0;
    }
    return q->count;
}

/* =============================================================================
 * CORE QUEUE OPERATIONS
 * =============================================================================
 */

bool cq_enqueue(CircularQueue *q, int item, QueueStats *stats) {
    if (!q) {
        return false;
    }

    if (cq_is_full(q)) {
        if (stats) {
            stats->overflow_count++;
        }
        return false;
    }

    q->data[q->rear] = item;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;

    if (stats) {
        stats->total_enqueues++;
    }

    return true;
}

bool cq_dequeue(CircularQueue *q, int *item, QueueStats *stats) {
    if (!q || !item) {
        return false;
    }

    if (cq_is_empty(q)) {
        if (stats) {
            stats->underflow_count++;
        }
        return false;
    }

    *item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;

    if (stats) {
        stats->total_dequeues++;
    }

    return true;
}

bool cq_peek(const CircularQueue *q, int *item) {
    if (!q || !item) {
        return false;
    }

    if (cq_is_empty(q)) {
        return false;
    }

    *item = q->data[q->front];
    return true;
}

/* =============================================================================
 * DISPLAY FUNCTIONS
 * =============================================================================
 */

void cq_display(const CircularQueue *q) {
    if (!q) {
        printf("Queue is NULL\n");
        return;
    }

    printf("Queue: ");

    for (int i = 0; i < q->capacity; i++) {
        int idx = (q->front + i) % q->capacity;

        if (i < q->count) {
            printf("[%3d] ", q->data[idx]);
        } else {
            printf("[   ] ");
        }
    }
    printf("\n");

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

void cq_display_circular(const CircularQueue *q) {
    if (!q) {
        return;
    }

    printf("\n  Circular Buffer Visualisation:\n");
    printf("  Capacity: %d, Count: %d\n", q->capacity, q->count);
    printf("  Front index: %d, Rear index: %d\n\n", q->front, q->rear);

    printf("     ");
    for (int i = 0; i < q->capacity; i++) {
        printf("┌─────┐");
    }
    printf("\n     ");

    for (int i = 0; i < q->capacity; i++) {
        bool has_data = false;
        int value = 0;

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

    for (int i = 0; i < q->capacity; i++) {
        char marker = ' ';
        if (i == q->front && i == q->rear) {
            marker = '*';
        } else if (i == q->front) {
            marker = 'F';
        } else if (i == q->rear) {
            marker = 'R';
        }
        printf("  [%d]%c ", i, marker);
    }
    printf("\n");
    printf("  Legend: F=Front, R=Rear, *=Both\n\n");
}

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

void process_commands(CircularQueue *q, QueueStats *stats, bool interactive) {
    char command[MAX_COMMAND_LENGTH];
    char operation[MAX_COMMAND_LENGTH];
    int value;
    int item;

    if (interactive) {
        printf("\nCircular Queue Interactive Mode\n");
        printf("Commands: ENQUEUE <n>, DEQUEUE, PEEK, SIZE, DISPLAY, CIRCULAR, STATS, QUIT\n");
        printf("─────────────────────────────────────────────────────────────────────────\n\n");
    }

    while (1) {
        if (interactive) {
            printf("> ");
            fflush(stdout);
        }

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = 0;
        command[strcspn(command, "\r")] = 0;

        if (command[0] == '\0') {
            continue;
        }

        if (sscanf(command, "%63s %d", operation, &value) >= 1) {

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
            } else if (strcmp(operation, "DEQUEUE") == 0) {
                if (cq_dequeue(q, &item, stats)) {
                    printf("Dequeued: %d\n", item);
                } else {
                    printf("Error: Queue is empty (underflow)\n");
                }
            } else if (strcmp(operation, "PEEK") == 0) {
                if (cq_peek(q, &item)) {
                    printf("Front element: %d\n", item);
                } else {
                    printf("Error: Queue is empty\n");
                }
            } else if (strcmp(operation, "SIZE") == 0) {
                printf("Queue size: %d / %d\n", cq_size(q), q->capacity);
            } else if (strcmp(operation, "DISPLAY") == 0) {
                cq_display(q);
            } else if (strcmp(operation, "CIRCULAR") == 0) {
                cq_display_circular(q);
            } else if (strcmp(operation, "STATS") == 0) {
                print_stats(stats);
            } else if (strcmp(operation, "QUIT") == 0 || strcmp(operation, "EXIT") == 0) {
                printf("Goodbye!\n");
                break;
            } else if (strcmp(operation, "HELP") == 0) {
                printf("Available commands:\n");
                printf("  ENQUEUE <n>  - Add value to rear of queue\n");
                printf("  DEQUEUE      - Remove and show front element\n");
                printf("  PEEK         - Show front element without removal\n");
                printf("  SIZE         - Show current queue size\n");
                printf("  DISPLAY      - Show linear queue view\n");
                printf("  CIRCULAR     - Show circular buffer view\n");
                printf("  STATS        - Show operation statistics\n");
                printf("  QUIT         - Exit program\n");
            } else {
                printf("Unknown command: %s (type HELP for list)\n", operation);
            }
        }
    }
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    bool interactive = stdin_is_tty();

    /* Optional explicit quiet/test flags for scripting convenience. */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test") == 0 || strcmp(argv[i], "--quiet") == 0) {
            interactive = false;
        }
    }

    if (interactive) {
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║     EXERCISE 1: CIRCULAR BUFFER QUEUE                         ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n");
    }

    CircularQueue *queue = cq_create(DEFAULT_CAPACITY);

    if (!queue) {
        fprintf(stderr, "Error: Failed to create queue\n");
        return 1;
    }

    QueueStats stats = (QueueStats){0, 0, 0, 0};

    process_commands(queue, &stats, interactive);

    if (interactive) {
        printf("\nFinal Statistics:\n");
        print_stats(&stats);
    }

    cq_destroy(queue);
    return 0;
}
