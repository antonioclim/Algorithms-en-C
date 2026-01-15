/**
 * =============================================================================
 * EXERCISE 2 - SOLUTION: BELLMAN-FORD ALGORITHM WITH ARBITRAGE DETECTION
 * =============================================================================
 *
 * This file contains the complete solution for Exercise 2.
 * For instructor use only - do not distribute to students.
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -lm -o exercise2_sol exercise2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>

#define MAX_CURRENCIES 100
#define MAX_NAME_LEN 16
#define INF DBL_MAX

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 1: Edge structure with logarithmic weight transformation
 */
typedef struct {
    int src;
    int dest;
    double weight;
    double original_rate;
} Edge;

/**
 * SOLUTION TODO 2: Currency graph structure
 */
typedef struct {
    int V;
    int E;
    Edge *edges;
    char names[MAX_CURRENCIES][MAX_NAME_LEN];
} CurrencyGraph;

/* =============================================================================
 * GRAPH FUNCTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 3: Create a currency graph with V currencies
 */
CurrencyGraph *create_currency_graph(int V) {
    CurrencyGraph *g = (CurrencyGraph *)malloc(sizeof(CurrencyGraph));
    if (!g) {
        return NULL;
    }
    
    g->V = V;
    g->E = 0;
    
    /* Maximum edges: V * (V-1) for a complete directed graph */
    g->edges = (Edge *)malloc(V * (V - 1) * sizeof(Edge));
    if (!g->edges) {
        free(g);
        return NULL;
    }
    
    return g;
}

/**
 * SOLUTION TODO 4: Add an edge with transformed weight -log(rate)
 * 
 * The transformation allows us to convert multiplication to addition:
 *   rate1 * rate2 * ... * rateN > 1  (profitable cycle)
 * becomes:
 *   -log(rate1) + -log(rate2) + ... + -log(rateN) < 0  (negative cycle)
 */
void add_currency_edge(CurrencyGraph *g, int src, int dest, double rate) {
    /* Skip invalid edges */
    if (rate <= 0 || src == dest) {
        return;
    }
    
    /* Store edge data */
    Edge *edge = &g->edges[g->E];
    edge->src = src;
    edge->dest = dest;
    edge->original_rate = rate;
    
    /* Transform: -log(rate) makes profitable exchanges negative weights */
    edge->weight = -log(rate);
    
    g->E++;
}

/**
 * Free graph memory
 */
void free_currency_graph(CurrencyGraph *g) {
    if (g) {
        free(g->edges);
        free(g);
    }
}

/* =============================================================================
 * BELLMAN-FORD ALGORITHM
 * =============================================================================
 */

/**
 * SOLUTION TODO 5: Run V-1 iterations of Bellman-Ford
 */
void bellman_ford_distances(CurrencyGraph *g, int source, 
                            double dist[], int parent[]) {
    /* Initialisation */
    for (int i = 0; i < g->V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[source] = 0;
    
    /* Relax all edges V-1 times */
    for (int iteration = 0; iteration < g->V - 1; iteration++) {
        bool updated = false;
        
        /* Process all edges */
        for (int e = 0; e < g->E; e++) {
            int u = g->edges[e].src;
            int v = g->edges[e].dest;
            double w = g->edges[e].weight;
            
            /* Relaxation step */
            if (dist[u] != INF && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                updated = true;
            }
        }
        
        /* Early termination if no updates */
        if (!updated) {
            break;
        }
    }
}

/**
 * SOLUTION TODO 6: Check for negative cycle (one more iteration)
 */
bool detect_negative_cycle(CurrencyGraph *g, double dist[], 
                           int parent[] __attribute__((unused)), 
                           int *cycle_vertex) {
    /* One more relaxation pass */
    for (int e = 0; e < g->E; e++) {
        int u = g->edges[e].src;
        int v = g->edges[e].dest;
        double w = g->edges[e].weight;
        
        /* If we can still relax, there's a negative cycle */
        if (dist[u] != INF && dist[u] + w < dist[v]) {
            *cycle_vertex = v;
            return true;
        }
    }
    
    return false;
}

/**
 * SOLUTION TODO 7: Trace back to find the actual cycle
 */
void find_cycle(CurrencyGraph *g, int start_vertex, int parent[],
                int cycle[], int *cycle_len) {
    /* Walk back V steps to ensure we're in the cycle */
    int vertex = start_vertex;
    for (int i = 0; i < g->V; i++) {
        vertex = parent[vertex];
    }
    
    /* This vertex is guaranteed to be in the cycle */
    int cycle_start = vertex;
    
    /* Collect cycle vertices */
    *cycle_len = 0;
    do {
        cycle[*cycle_len] = vertex;
        (*cycle_len)++;
        vertex = parent[vertex];
    } while (vertex != cycle_start && *cycle_len < g->V);
    
    /* Reverse the cycle to get correct order */
    for (int i = 0; i < *cycle_len / 2; i++) {
        int temp = cycle[i];
        cycle[i] = cycle[*cycle_len - 1 - i];
        cycle[*cycle_len - 1 - i] = temp;
    }
}

/* =============================================================================
 * ARBITRAGE CALCULATION
 * =============================================================================
 */

/**
 * SOLUTION TODO 8: Calculate profit percentage from cycle
 */
double calculate_profit(CurrencyGraph *g, int cycle[], int cycle_len) {
    double product = 1.0;
    
    /* Multiply all exchange rates in the cycle */
    for (int i = 0; i < cycle_len; i++) {
        int from = cycle[i];
        int to = cycle[(i + 1) % cycle_len];
        
        /* Find the edge from 'from' to 'to' */
        for (int e = 0; e < g->E; e++) {
            if (g->edges[e].src == from && g->edges[e].dest == to) {
                product *= g->edges[e].original_rate;
                break;
            }
        }
    }
    
    /* Return profit as percentage */
    return (product - 1.0) * 100.0;
}

/* =============================================================================
 * OUTPUT FUNCTIONS
 * =============================================================================
 */

/**
 * SOLUTION TODO 9: Print the arbitrage opportunity
 */
void print_arbitrage_cycle(CurrencyGraph *g, int cycle[], int cycle_len) {
    printf("Arbitrage opportunity detected!\n\n");
    
    /* Print cycle */
    printf("Cycle: ");
    for (int i = 0; i < cycle_len; i++) {
        printf("%s", g->names[cycle[i]]);
        printf(" -> ");
    }
    printf("%s\n\n", g->names[cycle[0]]);
    
    /* Print detailed exchange breakdown */
    printf("Exchange sequence:\n");
    double amount = 1000.0;
    for (int i = 0; i < cycle_len; i++) {
        int from = cycle[i];
        int to = cycle[(i + 1) % cycle_len];
        
        /* Find the rate */
        double rate = 0;
        for (int e = 0; e < g->E; e++) {
            if (g->edges[e].src == from && g->edges[e].dest == to) {
                rate = g->edges[e].original_rate;
                break;
            }
        }
        
        double new_amount = amount * rate;
        printf("  %.2f %s -> %.2f %s (rate: %.4f)\n",
               amount, g->names[from],
               new_amount, g->names[to],
               rate);
        amount = new_amount;
    }
    
    /* Print profit */
    double profit = calculate_profit(g, cycle, cycle_len);
    printf("\nProfit: %.2f%%\n", profit);
}

/* =============================================================================
 * MAIN DETECTION FUNCTION
 * =============================================================================
 */

/**
 * SOLUTION TODO 10: Main arbitrage detection function
 */
bool detect_arbitrage(CurrencyGraph *g) {
    double *dist = (double *)malloc(g->V * sizeof(double));
    int *parent = (int *)malloc(g->V * sizeof(int));
    int *cycle = (int *)malloc(g->V * sizeof(int));
    
    if (!dist || !parent || !cycle) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(dist);
        free(parent);
        free(cycle);
        return false;
    }
    
    bool found = false;
    
    /* Check from each starting vertex (complete check) */
    for (int source = 0; source < g->V && !found; source++) {
        /* Run Bellman-Ford */
        bellman_ford_distances(g, source, dist, parent);
        
        /* Check for negative cycle */
        int cycle_vertex;
        if (detect_negative_cycle(g, dist, parent, &cycle_vertex)) {
            /* Find and print the cycle */
            int cycle_len;
            find_cycle(g, cycle_vertex, parent, cycle, &cycle_len);
            print_arbitrage_cycle(g, cycle, cycle_len);
            found = true;
        }
    }
    
    if (!found) {
        printf("No arbitrage opportunity found.\n");
    }
    
    free(dist);
    free(parent);
    free(cycle);
    
    return found;
}

/* =============================================================================
 * INPUT PARSING
 * =============================================================================
 */

/**
 * SOLUTION TODO 11: Read currency graph from stdin
 */
CurrencyGraph *read_currency_graph(void) {
    int N;
    if (scanf("%d", &N) != 1 || N <= 0 || N > MAX_CURRENCIES) {
        fprintf(stderr, "Error: Invalid number of currencies\n");
        return NULL;
    }
    
    CurrencyGraph *g = create_currency_graph(N);
    if (!g) {
        fprintf(stderr, "Error: Failed to create graph\n");
        return NULL;
    }
    
    /* Read currency names */
    for (int i = 0; i < N; i++) {
        if (scanf("%s", g->names[i]) != 1) {
            fprintf(stderr, "Error: Failed to read currency name\n");
            free_currency_graph(g);
            return NULL;
        }
    }
    
    /* Read exchange rate matrix */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double rate;
            if (scanf("%lf", &rate) != 1) {
                fprintf(stderr, "Error: Failed to read exchange rate\n");
                free_currency_graph(g);
                return NULL;
            }
            
            /* Add edge if it's not a self-loop */
            if (i != j) {
                add_currency_edge(g, i, j, rate);
            }
        }
    }
    
    return g;
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║       Currency Arbitrage Detection using Bellman-Ford      ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    /**
     * SOLUTION TODO 12: Main program flow
     */
    CurrencyGraph *g = read_currency_graph();
    if (!g) {
        return EXIT_FAILURE;
    }
    
    /* Print loaded currencies */
    printf("Loaded %d currencies: ", g->V);
    for (int i = 0; i < g->V; i++) {
        printf("%s", g->names[i]);
        if (i < g->V - 1) printf(", ");
    }
    printf("\n");
    printf("Total edges: %d\n\n", g->E);
    
    /* Detect arbitrage */
    detect_arbitrage(g);
    
    /* Cleanup */
    free_currency_graph(g);
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * END OF SOLUTION
 * =============================================================================
 */
