/**
 * =============================================================================
 * EXERCISE 1: CIRCULAR BUFFER QUEUE - SOLUTION
 * =============================================================================
 *
 * Complete solution demonstrating proper circular buffer implementation.
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_CAPACITY 8
#define MAX_COMMAND_LENGTH 64

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct {
    int *data;
    int front;
    int rear;
    int count;
    int capacity;
} CircularQueue;

typedef struct {
    int total_enqueues;
    int total_dequeues;
    int overflow_count;
    int underflow_count;
} QueueStats;

/* =============================================================================
 * QUEUE IMPLEMENTATION
 * =============================================================================
 */

CircularQueue *cq_create(int capacity) {
    CircularQueue *q = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (!q) return NULL;
    
    q->data = (int *)malloc(sizeof(int) * capacity);
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
    if (q) {
        free(q->data);
        free(q);
    }
}

bool cq_is_empty(const CircularQueue *q) {
    return q->count == 0;
}

bool cq_is_full(const CircularQueue *q) {
    return q->count == q->capacity;
}

int cq_size(const CircularQueue *q) {
    if (!q) return 0;
    return q->count;
}

bool cq_enqueue(CircularQueue *q, int item, QueueStats *stats) {
    if (!q) return false;
    
    if (cq_is_full(q)) {
        if (stats) stats->overflow_count++;
        return false;
    }
    
    q->data[q->rear] = item;
    q->rear = (q->rear + 1) % q->capacity;
    q->count++;
    
    if (stats) stats->total_enqueues++;
    return true;
}

bool cq_dequeue(CircularQueue *q, int *item, QueueStats *stats) {
    if (!q || !item) return false;
    
    if (cq_is_empty(q)) {
        if (stats) stats->underflow_count++;
        return false;
    }
    
    *item = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    
    if (stats) stats->total_dequeues++;
    return true;
}

bool cq_peek(const CircularQueue *q, int *item) {
    if (!q || !item) return false;
    
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
    if (!q) return;
    
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
        
        command[strcspn(command, "\n")] = 0;
        
        if (strlen(command) == 0) continue;
        
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
    printf("║     EXERCISE 1: CIRCULAR BUFFER QUEUE - SOLUTION              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    CircularQueue *queue = cq_create(DEFAULT_CAPACITY);
    
    if (!queue) {
        fprintf(stderr, "Error: Failed to create queue\n");
        return 1;
    }
    
    QueueStats stats = {0, 0, 0, 0};
    
    process_commands(queue, &stats);
    
    printf("\nFinal Statistics:\n");
    print_stats(&stats);
    
    cq_destroy(queue);
    
    return 0;
}
