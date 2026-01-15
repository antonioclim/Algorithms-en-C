/**
 * =============================================================================
 * HOMEWORK 2 SOLUTION: Task Scheduler with Dependencies
 * =============================================================================
 *
 * INSTRUCTOR SOLUTION - DO NOT DISTRIBUTE TO STUDENTS
 *
 * This solution implements:
 *   1. Task graph construction with directed edges
 *   2. Cycle detection using three-colour DFS
 *   3. Topological sort for valid execution order
 *   4. Critical path analysis for minimum completion time
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTS
 * =============================================================================
 */

#define MAX_TASKS 10000

typedef enum {
    WHITE = 0,
    GREY = 1,
    BLACK = 2
} Colour;

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

typedef struct DepNode {
    int task_id;
    struct DepNode *next;
} DepNode;

typedef struct {
    int num_tasks;
    int num_deps;
    DepNode *successors[MAX_TASKS];   /* Tasks that depend on this task */
    DepNode *predecessors[MAX_TASKS]; /* Tasks this task depends on */
    int in_degree[MAX_TASKS];
    int out_degree[MAX_TASKS];
} TaskGraph;

typedef struct {
    int items[MAX_TASKS];
    int top;
} Stack;

/* =============================================================================
 * STACK OPERATIONS
 * =============================================================================
 */

void stack_init(Stack *s) {
    s->top = -1;
}

bool stack_is_empty(Stack *s) {
    return s->top == -1;
}

void stack_push(Stack *s, int value) {
    s->items[++s->top] = value;
}

int stack_pop(Stack *s) {
    return s->items[s->top--];
}

/* =============================================================================
 * GRAPH OPERATIONS
 * =============================================================================
 */

void taskgraph_init(TaskGraph *g, int n) {
    g->num_tasks = n;
    g->num_deps = 0;
    
    for (int i = 0; i < n; i++) {
        g->successors[i] = NULL;
        g->predecessors[i] = NULL;
        g->in_degree[i] = 0;
        g->out_degree[i] = 0;
    }
}

DepNode *create_dep_node(int task_id) {
    DepNode *node = (DepNode *)malloc(sizeof(DepNode));
    if (node == NULL) {
        return NULL;
    }
    
    node->task_id = task_id;
    node->next = NULL;
    
    return node;
}

/* Add dependency: task u must complete before task v */
void taskgraph_add_dependency(TaskGraph *g, int u, int v) {
    /* v is a successor of u (u -> v) */
    DepNode *succ = create_dep_node(v);
    if (succ != NULL) {
        succ->next = g->successors[u];
        g->successors[u] = succ;
        g->out_degree[u]++;
    }
    
    /* u is a predecessor of v */
    DepNode *pred = create_dep_node(u);
    if (pred != NULL) {
        pred->next = g->predecessors[v];
        g->predecessors[v] = pred;
        g->in_degree[v]++;
    }
    
    g->num_deps++;
}

void taskgraph_destroy(TaskGraph *g) {
    for (int i = 0; i < g->num_tasks; i++) {
        DepNode *curr = g->successors[i];
        while (curr != NULL) {
            DepNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        g->successors[i] = NULL;
        
        curr = g->predecessors[i];
        while (curr != NULL) {
            DepNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        g->predecessors[i] = NULL;
    }
    
    g->num_tasks = 0;
    g->num_deps = 0;
}

/* =============================================================================
 * CYCLE DETECTION
 * =============================================================================
 */

bool detect_cycle_dfs(TaskGraph *g, int v, Colour colour[], 
                      int cycle_path[], int *cycle_len, int parent[]) {
    colour[v] = GREY;
    
    DepNode *curr = g->successors[v];
    while (curr != NULL) {
        int u = curr->task_id;
        
        if (colour[u] == GREY) {
            /* Found cycle - reconstruct it */
            *cycle_len = 0;
            cycle_path[(*cycle_len)++] = u;
            
            int trace = v;
            while (trace != u && *cycle_len < MAX_TASKS) {
                cycle_path[(*cycle_len)++] = trace;
                trace = parent[trace];
            }
            
            return true;
        }
        
        if (colour[u] == WHITE) {
            parent[u] = v;
            if (detect_cycle_dfs(g, u, colour, cycle_path, cycle_len, parent)) {
                return true;
            }
        }
        
        curr = curr->next;
    }
    
    colour[v] = BLACK;
    return false;
}

bool has_cycle(TaskGraph *g, int cycle_path[], int *cycle_len) {
    Colour colour[MAX_TASKS];
    int parent[MAX_TASKS];
    
    for (int i = 0; i < g->num_tasks; i++) {
        colour[i] = WHITE;
        parent[i] = -1;
    }
    
    *cycle_len = 0;
    
    for (int v = 0; v < g->num_tasks; v++) {
        if (colour[v] == WHITE) {
            if (detect_cycle_dfs(g, v, colour, cycle_path, cycle_len, parent)) {
                return true;
            }
        }
    }
    
    return false;
}

/* =============================================================================
 * TOPOLOGICAL SORT (Kahn's Algorithm)
 * =============================================================================
 */

int topological_sort(TaskGraph *g, int order[]) {
    int in_deg[MAX_TASKS];
    int queue[MAX_TASKS];
    int front = 0, rear = -1;
    int count = 0;
    
    /* Copy in-degrees */
    for (int i = 0; i < g->num_tasks; i++) {
        in_deg[i] = g->in_degree[i];
        
        /* Enqueue all tasks with no dependencies */
        if (in_deg[i] == 0) {
            queue[++rear] = i;
        }
    }
    
    /* Process queue */
    while (front <= rear) {
        int u = queue[front++];
        order[count++] = u;
        
        /* Reduce in-degree of all successors */
        DepNode *curr = g->successors[u];
        while (curr != NULL) {
            int v = curr->task_id;
            in_deg[v]--;
            
            if (in_deg[v] == 0) {
                queue[++rear] = v;
            }
            
            curr = curr->next;
        }
    }
    
    /* If not all tasks processed, there's a cycle */
    return (count == g->num_tasks) ? count : -1;
}

/* =============================================================================
 * CRITICAL PATH ANALYSIS
 * =============================================================================
 */

void find_critical_path(TaskGraph *g, int topo_order[], int n) {
    /* Each task takes 1 time unit */
    int earliest_start[MAX_TASKS];
    int latest_start[MAX_TASKS];
    int parent_on_path[MAX_TASKS];
    
    /* Forward pass: compute earliest start times */
    for (int i = 0; i < n; i++) {
        earliest_start[i] = 0;
        parent_on_path[i] = -1;
    }
    
    for (int i = 0; i < n; i++) {
        int u = topo_order[i];
        
        DepNode *curr = g->successors[u];
        while (curr != NULL) {
            int v = curr->task_id;
            
            if (earliest_start[u] + 1 > earliest_start[v]) {
                earliest_start[v] = earliest_start[u] + 1;
                parent_on_path[v] = u;
            }
            
            curr = curr->next;
        }
    }
    
    /* Find maximum completion time (critical path length) */
    int max_time = 0;
    int end_task = -1;
    
    for (int i = 0; i < n; i++) {
        if (earliest_start[i] + 1 > max_time) {
            max_time = earliest_start[i] + 1;
            end_task = i;
        }
    }
    
    printf("Critical path length: %d\n", max_time);
    
    /* Backward pass: compute latest start times */
    for (int i = 0; i < n; i++) {
        latest_start[i] = max_time - 1;
    }
    
    for (int i = n - 1; i >= 0; i--) {
        int u = topo_order[i];
        
        DepNode *curr = g->successors[u];
        while (curr != NULL) {
            int v = curr->task_id;
            
            if (latest_start[v] - 1 < latest_start[u]) {
                latest_start[u] = latest_start[v] - 1;
            }
            
            curr = curr->next;
        }
    }
    
    /* Tasks on critical path have earliest_start == latest_start */
    printf("Critical tasks: ");
    bool first = true;
    for (int i = 0; i < n; i++) {
        int task = topo_order[i];
        if (earliest_start[task] == latest_start[task]) {
            if (!first) printf(" -> ");
            printf("%d", task);
            first = false;
        }
    }
    printf("\n");
    
    /* Also show a specific critical path by backtracking */
    printf("One critical path: ");
    
    /* Find a task with max completion time and backtrack */
    int path[MAX_TASKS];
    int path_len = 0;
    
    int current = end_task;
    while (current != -1) {
        path[path_len++] = current;
        current = parent_on_path[current];
    }
    
    /* Print in forward order */
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    TaskGraph g;
    int n, m;
    
    /* Read number of tasks and dependencies */
    if (scanf("%d %d", &n, &m) != 2) {
        fprintf(stderr, "Error: Failed to read graph dimensions\n");
        return 1;
    }
    
    if (n <= 0 || n > MAX_TASKS) {
        fprintf(stderr, "Error: Invalid number of tasks\n");
        return 1;
    }
    
    taskgraph_init(&g, n);
    
    /* Read dependencies */
    for (int i = 0; i < m; i++) {
        int u, v;
        if (scanf("%d %d", &u, &v) != 2) {
            fprintf(stderr, "Error: Failed to read dependency %d\n", i + 1);
            taskgraph_destroy(&g);
            return 1;
        }
        
        if (u < 0 || u >= n || v < 0 || v >= n) {
            fprintf(stderr, "Error: Invalid task ID in dependency %d\n", i + 1);
            continue;
        }
        
        taskgraph_add_dependency(&g, u, v);
    }
    
    /* Check for cycles */
    int cycle_path[MAX_TASKS];
    int cycle_len = 0;
    
    if (has_cycle(&g, cycle_path, &cycle_len)) {
        printf("Cycle detected - scheduling impossible!\n");
        printf("Tasks in cycle: ");
        for (int i = 0; i < cycle_len; i++) {
            printf("%d", cycle_path[i]);
            if (i < cycle_len - 1) printf(" -> ");
        }
        printf(" -> %d\n", cycle_path[0]);
        
        taskgraph_destroy(&g);
        return 1;
    }
    
    printf("No cycles detected - scheduling possible\n\n");
    
    /* Perform topological sort */
    int order[MAX_TASKS];
    int order_len = topological_sort(&g, order);
    
    if (order_len > 0) {
        printf("Execution order: ");
        for (int i = 0; i < order_len; i++) {
            printf("%d", order[i]);
            if (i < order_len - 1) printf(" ");
        }
        printf("\n\n");
        
        /* Critical path analysis */
        find_critical_path(&g, order, order_len);
    }
    
    /* Clean up */
    taskgraph_destroy(&g);
    
    return 0;
}
