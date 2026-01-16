/**
 * =============================================================================
 * EXERCISE 2: BELLMAN-FORD ALGORITHM WITH ARBITRAGE DETECTION
 * =============================================================================
 *
 * OBJECTIVE:
 *   Implement the Bellman-Ford algorithm for finding shortest paths in graphs
 *   with negative edge weights. Apply it to detect arbitrage opportunities
 *   in currency exchange rates.
 *
 * REQUIREMENTS:
 *   1. Implement the Bellman-Ford algorithm
 *   2. Detect negative cycles (arbitrage opportunities)
 *   3. Reconstruct and print the arbitrage cycle if found
 *   4. Handle the transformation: -log(rate) for edge weights
 *
 * ARBITRAGE EXPLANATION:
 *   Currency arbitrage exists when you can trade currencies in a cycle
 *   and end up with more money than you started with.
 *   
 *   Example: 1 USD -> EUR -> GBP -> USD gives 1.01 USD (1% profit)
 *   
 *   We transform exchange rates using -log(rate) so that:
 *   - Multiplying rates becomes adding weights
 *   - A profitable cycle (product > 1) becomes a negative cycle (sum < 0)
 *
 * INPUT FORMAT:
 *   Line 1: N (number of currencies)
 *   Line 2: currency names separated by spaces
 *   Lines 3 to N+2: N exchange rates (row i, column j = rate from i to j)
 *
 * OUTPUT FORMAT:
 *   If arbitrage exists:
 *     "Arbitrage opportunity detected!"
 *     "Cycle: USD -> EUR -> GBP -> USD"
 *     "Profit: X.XX%"
 *   If no arbitrage:
 *     "No arbitrage opportunity found."
 *
 * EXAMPLE INPUT:
 *   4
 *   USD EUR GBP CHF
 *   1.0000 0.9200 0.7800 0.9100
 *   1.0870 1.0000 0.8500 0.9900
 *   1.2820 1.1765 1.0000 1.1650
 *   1.0989 1.0101 0.8584 1.0000
 *
 * COMPILATION: gcc -Wall -Wextra -std=c11 -lm -o exercise2 exercise2.c
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
#define EPSILON 1e-12

/* =============================================================================
 * TYPE DEFINITIONS
 * =============================================================================
 */

/**
 * TODO 1: Define the Edge structure
 *
 * An edge should contain:
 *   - src: source vertex (int)
 *   - dest: destination vertex (int)
 *   - weight: edge weight as double (for -log transformation)
 *   - original_rate: the original exchange rate (for output)
 *
 * Hint: We store weight as double because of logarithm transformation
 */

/* YOUR CODE HERE */
typedef struct {
    int src;
    int dest;
    double weight;
    double original_rate;
} Edge;

/**
 * TODO 2: Define the CurrencyGraph structure
 *
 * The graph should contain:
 *   - V: number of vertices (currencies)
 *   - E: number of edges
 *   - edges: array of Edge structures
 *   - names: array of currency name strings
 *
 * Hint: For N currencies, we have at most N*(N-1) directed edges
 */

/* YOUR CODE HERE */
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
 * TODO 3: Implement create_currency_graph
 *
 * @param V Number of currencies
 * @return Pointer to newly created graph
 *
 * Steps:
 *   1. Allocate memory for CurrencyGraph structure
 *   2. Set V to number of currencies
 *   3. Set E to 0 (will be incremented when adding edges)
 *   4. Allocate memory for edges array (V * (V-1) max edges)
 *   5. Return the graph pointer
 */
CurrencyGraph *create_currency_graph(int V) {

    if (V <= 0 || V > MAX_CURRENCIES) {
        return NULL;
    }

    CurrencyGraph *g = (CurrencyGraph *)malloc(sizeof(CurrencyGraph));
    if (!g) {
        return NULL;
    }

    g->V = V;
    g->E = 0;

    long long max_edges = (long long)V * (long long)(V - 1);
    if (max_edges <= 0 || max_edges > 10000000) {
        /* Defensive bound for accidental overflow or unreasonable inputs */
        free(g);
        return NULL;
    }

    g->edges = (Edge *)malloc((size_t)max_edges * sizeof(Edge));
    if (!g->edges) {
        free(g);
        return NULL;
    }

    /* Names are stored inside the struct so no allocation is required here */
    for (int i = 0; i < g->V; i++) {
        g->names[i][0] = '\0';
    }

    return g;
}

/**
 * TODO 4: Implement add_currency_edge
 *
 * Add an edge with transformed weight -log(rate).
 *
 * @param g Pointer to the graph
 * @param src Source currency index
 * @param dest Destination currency index
 * @param rate Exchange rate from src to dest
 *
 * Steps:
 *   1. Skip if rate <= 0 or src == dest
 *   2. Store source, destination and original rate
 *   3. Calculate weight as -log(rate)
 *   4. Increment edge count E
 *
 * Hint: A rate > 1 means you gain currency, giving negative weight
 *       which is what we want to detect as "negative cycle"
 */
void add_currency_edge(CurrencyGraph *g, int src, int dest, double rate) {

    if (!g) {
        return;
    }
    if (src < 0 || src >= g->V || dest < 0 || dest >= g->V) {
        return;
    }
    if (src == dest) {
        return;
    }
    if (rate <= 0.0) {
        return;
    }

    long long max_edges = (long long)g->V * (long long)(g->V - 1);
    if (g->E >= max_edges) {
        return;
    }

    Edge *e = &g->edges[g->E];
    e->src = src;
    e->dest = dest;
    e->original_rate = rate;
    e->weight = -log(rate);

    g->E++;
}

/**
 * Free graph memory
 * (Already implemented for you)
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
 * TODO 5: Implement bellman_ford_distances
 *
 * Run the main Bellman-Ford loop (V-1 iterations) to compute distances.
 *
 * @param g Pointer to the graph
 * @param source Source vertex
 * @param dist Output array for distances (double)
 * @param parent Output array for path reconstruction
 *
 * Steps:
 *   1. Initialise all distances to INF
 *   2. Initialise all parents to -1
 *   3. Set dist[source] = 0
 *   4. Repeat V-1 times:
 *      For each edge (u, v) with weight w:
 *        If dist[u] != INF AND dist[u] + w < dist[v]:
 *          Update dist[v] = dist[u] + w
 *          Update parent[v] = u
 *   5. Return nothing (results in dist and parent arrays)
 *
 * Hint: Use g->edges array to iterate through all edges
 */
void bellman_ford_distances(CurrencyGraph *g, int source, 
                            double dist[], int parent[]) {

    if (!g || source < 0 || source >= g->V) {
        return;
    }

    for (int i = 0; i < g->V; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[source] = 0.0;

    for (int iter = 0; iter < g->V - 1; iter++) {
        bool updated = false;

        for (int j = 0; j < g->E; j++) {
            Edge *e = &g->edges[j];
            int u = e->src;
            int v = e->dest;
            double w = e->weight;

            if (dist[u] != INF && dist[u] + w < dist[v] - EPSILON) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                updated = true;
            }
        }

        if (!updated) {
            break;
        }
    }
}

/**
 * TODO 6: Implement detect_negative_cycle
 *
 * After V-1 iterations, check if any edge can still be relaxed.
 * If so, a negative cycle exists.
 *
 * @param g Pointer to the graph
 * @param dist Distance array from bellman_ford_distances
 * @param parent Parent array from bellman_ford_distances
 * @param cycle_vertex Output: a vertex on the negative cycle (-1 if none)
 * @return true if negative cycle detected, false otherwise
 *
 * Steps:
 *   1. For each edge (u, v) with weight w:
 *      If dist[u] != INF AND dist[u] + w < dist[v]:
 *        Set *cycle_vertex = v
 *        Return true
 *   2. Return false (no negative cycle)
 *
 * Hint: The returned vertex is on or reachable from the cycle
 */
bool detect_negative_cycle(CurrencyGraph *g, double dist[], 
                           int parent[], int *cycle_vertex) {

    if (!g || !cycle_vertex) {
        return false;
    }
    *cycle_vertex = -1;

    for (int j = 0; j < g->E; j++) {
        Edge *e = &g->edges[j];
        int u = e->src;
        int v = e->dest;
        double w = e->weight;

        if (dist[u] != INF && dist[u] + w < dist[v] - EPSILON) {
            /* Store last relaxation to improve reconstruction quality */
            dist[v] = dist[u] + w;
            parent[v] = u;
            *cycle_vertex = v;
            return true;
        }
    }

    return false;
}

/**
 * TODO 7: Implement find_cycle
 *
 * Given a vertex on/reachable from a negative cycle, trace back
 * to find the actual cycle vertices.
 *
 * @param g Pointer to the graph
 * @param start_vertex A vertex involved in the negative cycle
 * @param parent Parent array
 * @param cycle Output array to store cycle vertices
 * @param cycle_len Output: number of vertices in cycle
 *
 * Steps:
 *   1. Walk back V steps from start_vertex to ensure we're in the cycle
 *      (The cycle might be deeper in the path)
 *   2. Mark this as the cycle_start
 *   3. Walk through parents until we return to cycle_start
 *   4. Store each vertex in the cycle array
 *   5. Set *cycle_len to the number of vertices found
 *
 * Hint: After V steps backward, we're guaranteed to be inside the cycle
 */
void find_cycle(CurrencyGraph *g, int start_vertex, int parent[],
                int cycle[], int *cycle_len) {


    if (!cycle_len) {
        return;
    }
    *cycle_len = 0;

    if (start_vertex < 0) {
        return;
    }

    int v = start_vertex;

    /* Step back V times to ensure we are inside the cycle */
    for (int i = 0; i < g->V; i++) {
        if (parent[v] == -1) {
            return;
        }
        v = parent[v];
    }

    int cycle_start = v;

    int cur = cycle_start;
    int len = 0;
    do {
        if (len >= MAX_CURRENCIES) {
            /* Defensive stop in pathological cases */
            break;
        }
        cycle[len++] = cur;
        cur = parent[cur];
    } while (cur != -1 && cur != cycle_start);

    if (cur != cycle_start) {
        /* Failed to close the cycle */
        *cycle_len = 0;
        return;
    }

    /* The collected cycle follows parent pointers (reverse direction). */
    for (int i = 0; i < len / 2; i++) {
        int tmp = cycle[i];
        cycle[i] = cycle[len - 1 - i];
        cycle[len - 1 - i] = tmp;
    }

    *cycle_len = len;
}

/* =============================================================================
 * ARBITRAGE CALCULATION
 * =============================================================================
 */

/**
 * TODO 8: Implement calculate_profit
 *
 * Calculate the profit percentage from an arbitrage cycle.
 *
 * @param g Pointer to the graph
 * @param cycle Array of vertices in the cycle
 * @param cycle_len Number of vertices in the cycle
 * @return Profit as a percentage (e.g., 2.5 for 2.5% profit)
 *
 * Steps:
 *   1. Initialise product = 1.0
 *   2. For each consecutive pair in the cycle:
 *      - Find the edge from cycle[i] to cycle[i+1]
 *      - Multiply product by original_rate
 *   3. Don't forget: cycle goes back to start (cycle[len-1] to cycle[0])
 *   4. Return (product - 1.0) * 100.0
 *
 * Hint: You need to search for the edge with matching src/dest
 */
double calculate_profit(CurrencyGraph *g, int cycle[], int cycle_len) {

    if (!g || !cycle || cycle_len <= 1) {
        return 0.0;
    }

    double product = 1.0;

    for (int i = 0; i < cycle_len; i++) {
        int u = cycle[i];
        int v = cycle[(i + 1) % cycle_len];

        bool found = false;
        for (int j = 0; j < g->E; j++) {
            Edge *e = &g->edges[j];
            if (e->src == u && e->dest == v) {
                product *= e->original_rate;
                found = true;
                break;
            }
        }

        if (!found) {
            /* Edge not found: inconsistent graph representation */
            return 0.0;
        }
    }

    return (product - 1.0) * 100.0;
}

/* =============================================================================
 * OUTPUT FUNCTIONS
 * =============================================================================
 */

/**
 * TODO 9: Implement print_arbitrage_cycle
 *
 * Print the arbitrage opportunity in a readable format.
 *
 * @param g Pointer to the graph
 * @param cycle Array of vertices in the cycle
 * @param cycle_len Number of vertices in the cycle
 *
 * Steps:
 *   1. Print "Arbitrage opportunity detected!"
 *   2. Print "Cycle: " followed by currency names
 *      Format: USD -> EUR -> GBP -> USD
 *   3. Calculate and print profit percentage
 *      Format: "Profit: X.XX%"
 *
 * Hint: Use g->names[vertex] to get currency name
 */
void print_arbitrage_cycle(CurrencyGraph *g, int cycle[], int cycle_len) {

    if (!g || !cycle || cycle_len <= 1) {
        return;
    }

    printf("Arbitrage opportunity detected!\n");

    /* Rotate cycle to start at vertex 0 if present, else at smallest index. */
    int start_pos = 0;
    int best_vertex = cycle[0];
    for (int i = 0; i < cycle_len; i++) {
        if (cycle[i] == 0) {
            start_pos = i;
            best_vertex = 0;
            break;
        }
        if (cycle[i] < best_vertex) {
            best_vertex = cycle[i];
            start_pos = i;
        }
    }

    printf("Cycle: ");
    for (int k = 0; k < cycle_len; k++) {
        int idx = cycle[(start_pos + k) % cycle_len];
        printf("%s", g->names[idx]);
        printf(" -> ");
    }
    /* close the cycle */
    printf("%s\n", g->names[cycle[start_pos]]);

    double profit = calculate_profit(g, cycle, cycle_len);
    printf("Profit: %.2f%%\n", profit);
}

/* =============================================================================
 * MAIN DETECTION FUNCTION
 * =============================================================================
 */

/**
 * TODO 10: Implement detect_arbitrage
 *
 * Main function to detect and report arbitrage opportunities.
 *
 * @param g Pointer to the currency graph
 * @return true if arbitrage found, false otherwise
 *
 * Steps:
 *   1. Allocate dist and parent arrays (size V, doubles for dist)
 *   2. Run Bellman-Ford from vertex 0 (any vertex works)
 *   3. Check for negative cycle
 *   4. If found:
 *      - Find the actual cycle vertices
 *      - Print the arbitrage opportunity
 *      - Return true
 *   5. If not found:
 *      - Print "No arbitrage opportunity found."
 *      - Return false
 *   6. Free allocated memory
 *
 * Hint: Must check starting from each vertex for complete detection
 *       (simplified version: start from vertex 0)
 */
bool detect_arbitrage(CurrencyGraph *g) {

    if (!g) {
        return false;
    }

    double *dist = (double *)malloc((size_t)g->V * sizeof(double));
    int *parent = (int *)malloc((size_t)g->V * sizeof(int));
    if (!dist || !parent) {
        free(dist);
        free(parent);
        return false;
    }

    /* Run Bellman-Ford from vertex 0 (sufficient for complete graphs). */
    bellman_ford_distances(g, 0, dist, parent);

    int cycle_vertex = -1;
    if (detect_negative_cycle(g, dist, parent, &cycle_vertex)) {
        int cycle[MAX_CURRENCIES];
        int cycle_len = 0;
        find_cycle(g, cycle_vertex, parent, cycle, &cycle_len);

        if (cycle_len > 0) {
            print_arbitrage_cycle(g, cycle, cycle_len);
            free(dist);
            free(parent);
            return true;
        }
    }

    printf("No arbitrage opportunity found.\n");

    free(dist);
    free(parent);
    return false;
}

/* =============================================================================
 * INPUT PARSING
 * =============================================================================
 */

/**
 * TODO 11: Implement read_currency_graph
 *
 * Read the currency graph from standard input.
 *
 * @return Pointer to the created and populated graph
 *
 * Steps:
 *   1. Read N (number of currencies)
 *   2. Create graph with N vertices
 *   3. Read N currency names into g->names
 *   4. Read N x N exchange rate matrix
 *   5. For each rate[i][j] where i != j:
 *      Call add_currency_edge(g, i, j, rate)
 *   6. Return the graph
 *
 * Hint: Use scanf for numbers, %s for currency names
 */
CurrencyGraph *read_currency_graph(void) {

    int N;
    if (scanf("%d", &N) != 1) {
        return NULL;
    }

    if (N <= 0 || N > MAX_CURRENCIES) {
        return NULL;
    }

    CurrencyGraph *g = create_currency_graph(N);
    if (!g) {
        return NULL;
    }

    for (int i = 0; i < N; i++) {
        if (scanf("%15s", g->names[i]) != 1) {
            free_currency_graph(g);
            return NULL;
        }
        g->names[i][MAX_NAME_LEN - 1] = '\0';
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double rate;
            if (scanf("%lf", &rate) != 1) {
                free_currency_graph(g);
                return NULL;
            }
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

    CurrencyGraph *g = read_currency_graph();
    if (!g) {
        fprintf(stderr, "Error: Failed to read currency graph\n");
        return EXIT_FAILURE;
    }

    (void)detect_arbitrage(g);

    free_currency_graph(g);
    return EXIT_SUCCESS;
}

/* =============================================================================
 * BONUS CHALLENGES (Optional)
 * =============================================================================
 *
 * 1. Modify the algorithm to find ALL arbitrage cycles, not just one.
 *
 * 2. Find the arbitrage cycle with MAXIMUM profit.
 *
 * 3. Add transaction fees: modify edge weights to include a percentage
 *    fee for each exchange.
 *
 * 4. Implement a "best path" finder that finds the most profitable
 *    way to convert from currency A to currency B (not necessarily a cycle).
 *
 * 5. Read exchange rates from a CSV file with real-world data.
 *
 * =============================================================================
 */
