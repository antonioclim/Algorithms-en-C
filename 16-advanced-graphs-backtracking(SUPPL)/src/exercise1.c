/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Exercise 1: MST and Topological Sort
 * =============================================================================
 *
 * In this exercise, you will implement:
 *   1. Union-Find data structure with path compression
 *   2. Kruskal's algorithm for Minimum Spanning Tree
 *   3. Prim's algorithm for Minimum Spanning Tree
 *   4. Topological Sort using DFS (reverse postorder)
 *   5. Topological Sort using Kahn's algorithm (BFS)
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 * Testing: make test-ex1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * CONSTANTS AND STRUCTURES
 * =============================================================================
 */

#define MAX_VERTICES 100
#define MAX_EDGES    500

/**
 * Edge structure for graph representation
 */
typedef struct {
    int src;
    int dest;
    int weight;
} Edge;

/**
 * Union-Find (Disjoint Set Union) structure
 * Used for efficient cycle detection in Kruskal's algorithm
 */
typedef struct {
    int parent[MAX_VERTICES];
    int rank[MAX_VERTICES];
    int size;
} UnionFind;

/**
 * Graph structure supporting both edge list and adjacency representations
 */
typedef struct {
    int V;                              /* Number of vertices */
    int E;                              /* Number of edges */
    Edge edges[MAX_EDGES];              /* Edge list for Kruskal */
    int adj[MAX_VERTICES][MAX_VERTICES]; /* Adjacency matrix for Prim */
    int adj_list[MAX_VERTICES][MAX_VERTICES]; /* Adjacency list for topo sort */
    int adj_count[MAX_VERTICES];        /* Adjacency list sizes */
} Graph;

/* =============================================================================
 * PART 1: UNION-FIND DATA STRUCTURE
 * =============================================================================
 *
 * Union-Find (also called Disjoint Set Union or DSU) is a data structure that
 * tracks a set of elements partitioned into disjoint (non-overlapping) subsets.
 *
 * Operations:
 *   - Find(x): Determine which subset element x belongs to
 *   - Union(x, y): Merge the subsets containing x and y
 *
 * Optimisations:
 *   - Path compression: Flatten the tree during Find
 *   - Union by rank: Always attach smaller tree under larger one
 *
 * With both optimisations: O(α(n)) per operation, where α is the 
 * inverse Ackermann function (effectively constant for all practical n)
 */

/**
 * Initialise Union-Find structure
 * @param uf Pointer to UnionFind structure
 * @param n  Number of elements (0 to n-1)
 */
void uf_init(UnionFind *uf, int n) {
    uf->size = n;
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;  /* Each element is its own parent */
        uf->rank[i] = 0;    /* Initial rank is 0 */
    }
}

/**
 * Find the representative (root) of the set containing x
 * Uses path compression: makes every node point directly to the root
 *
 * @param uf Pointer to UnionFind structure
 * @param x  Element to find
 * @return   Representative of the set containing x
 *
 * TODO 1: Implement path compression
 *         When finding the root, make each visited node point to the root
 *         This flattens the tree for future operations
 *
 * HINT: Use recursion - after finding root, update parent[x] to root
 */
int uf_find(UnionFind *uf, int x) {
    /* TODO: Implement Find with path compression */
    /* Current naive implementation (no path compression): */
    while (uf->parent[x] != x) {
        x = uf->parent[x];
    }
    return x;
    
    /* HINT: Recursive implementation with path compression:
     * if (uf->parent[x] != x) {
     *     uf->parent[x] = uf_find(uf, uf->parent[x]);  // Path compression
     * }
     * return uf->parent[x];
     */
}

/**
 * Unite the sets containing x and y
 * Uses union by rank: attach smaller tree under larger tree
 *
 * @param uf Pointer to UnionFind structure
 * @param x  First element
 * @param y  Second element
 *
 * TODO 2: Implement union by rank
 *         Compare ranks of the two roots:
 *         - If ranks differ, attach smaller to larger (rank unchanged)
 *         - If ranks equal, arbitrarily choose and increment winner's rank
 *
 * HINT: First find roots of x and y
 *       If already same root, do nothing
 *       Otherwise, compare ranks and attach appropriately
 */
void uf_union(UnionFind *uf, int x, int y) {
    int root_x = uf_find(uf, x);
    int root_y = uf_find(uf, y);
    
    if (root_x == root_y) return;  /* Already in same set */
    
    /* TODO: Implement union by rank */
    /* Current naive implementation (always attach x under y): */
    uf->parent[root_x] = root_y;
    
    /* HINT: Compare ranks:
     * if (uf->rank[root_x] < uf->rank[root_y]) {
     *     uf->parent[root_x] = root_y;
     * } else if (uf->rank[root_x] > uf->rank[root_y]) {
     *     uf->parent[root_y] = root_x;
     * } else {
     *     uf->parent[root_y] = root_x;
     *     uf->rank[root_x]++;
     * }
     */
}

/**
 * Check if two elements are in the same set
 */
bool uf_connected(UnionFind *uf, int x, int y) {
    return uf_find(uf, x) == uf_find(uf, y);
}

/* =============================================================================
 * PART 2: KRUSKAL'S ALGORITHM
 * =============================================================================
 *
 * Kruskal's algorithm finds MST by:
 *   1. Sort all edges by weight (ascending)
 *   2. For each edge (in sorted order):
 *      - If it doesn't create a cycle, add it to MST
 *      - Use Union-Find to check for cycles efficiently
 *
 * Time Complexity: O(E log E) for sorting, O(E α(V)) for Union-Find
 * Space Complexity: O(V) for Union-Find structure
 */

/**
 * Comparison function for sorting edges by weight
 */
int compare_edges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

/**
 * Kruskal's Algorithm for Minimum Spanning Tree
 *
 * @param g           Pointer to graph
 * @param mst         Array to store MST edges
 * @param mst_size    Pointer to store number of MST edges
 * @param total_weight Pointer to store total MST weight
 *
 * TODO 3: Implement Kruskal's algorithm
 *         1. Sort edges by weight
 *         2. Iterate through sorted edges
 *         3. For each edge, check if it creates a cycle using Union-Find
 *         4. If no cycle, add edge to MST and union the vertices
 *         5. Stop when MST has V-1 edges
 *
 * HINT: Use qsort() for sorting
 *       Use uf_connected() to check for cycle
 *       Use uf_union() to merge components
 */
void kruskal_mst(Graph *g, Edge mst[], int *mst_size, int *total_weight) {
    /* TODO: Implement Kruskal's algorithm */
    
    /* Step 1: Initialise Union-Find */
    UnionFind uf;
    uf_init(&uf, g->V);
    
    /* Step 2: Sort edges by weight */
    /* HINT: qsort(g->edges, g->E, sizeof(Edge), compare_edges); */
    
    *mst_size = 0;
    *total_weight = 0;
    
    /* Step 3: Process edges in sorted order */
    /* HINT:
     * for (int i = 0; i < g->E && *mst_size < g->V - 1; i++) {
     *     int u = g->edges[i].src;
     *     int v = g->edges[i].dest;
     *     
     *     // Check if adding this edge creates a cycle
     *     if (!uf_connected(&uf, u, v)) {
     *         // Add edge to MST
     *         mst[*mst_size] = g->edges[i];
     *         (*mst_size)++;
     *         *total_weight += g->edges[i].weight;
     *         
     *         // Merge the components
     *         uf_union(&uf, u, v);
     *     }
     * }
     */
    
    printf("[TODO] Kruskal's algorithm not implemented\n");
}

/* =============================================================================
 * PART 3: PRIM'S ALGORITHM
 * =============================================================================
 *
 * Prim's algorithm finds MST by:
 *   1. Start from an arbitrary vertex
 *   2. Repeatedly add the minimum-weight edge connecting the tree to a new vertex
 *
 * This implementation uses O(V²) adjacency matrix approach
 * (For sparse graphs, use priority queue for O(E log V))
 */

/**
 * Prim's Algorithm for Minimum Spanning Tree
 *
 * @param g            Pointer to graph (uses adjacency matrix)
 * @param start        Starting vertex
 * @param mst          Array to store MST edges
 * @param mst_size     Pointer to store number of MST edges
 * @param total_weight Pointer to store total MST weight
 *
 * TODO 4: Implement Prim's algorithm
 *         1. Maintain arrays: in_mst[], key[], parent[]
 *         2. key[v] = minimum weight edge to reach v from MST
 *         3. Repeat V times:
 *            - Find minimum key vertex not in MST
 *            - Add it to MST
 *            - Update keys of adjacent vertices
 *
 * HINT: Initialise key[start] = 0, all others = INT_MAX
 *       parent[start] = -1
 */
void prim_mst(Graph *g, int start, Edge mst[], int *mst_size, int *total_weight) {
    /* TODO: Implement Prim's algorithm */
    
    bool in_mst[MAX_VERTICES];
    int key[MAX_VERTICES];
    int parent[MAX_VERTICES];
    
    /* Initialise arrays */
    for (int i = 0; i < g->V; i++) {
        in_mst[i] = false;
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    key[start] = 0;
    
    *mst_size = 0;
    *total_weight = 0;
    
    /* HINT: Main loop
     * for (int count = 0; count < g->V; count++) {
     *     // Find minimum key vertex not in MST
     *     int min_key = INT_MAX, u = -1;
     *     for (int v = 0; v < g->V; v++) {
     *         if (!in_mst[v] && key[v] < min_key) {
     *             min_key = key[v];
     *             u = v;
     *         }
     *     }
     *     
     *     if (u == -1) break;  // No more reachable vertices
     *     
     *     in_mst[u] = true;
     *     
     *     // Add edge to MST (if not start vertex)
     *     if (parent[u] != -1) {
     *         mst[*mst_size].src = parent[u];
     *         mst[*mst_size].dest = u;
     *         mst[*mst_size].weight = key[u];
     *         (*mst_size)++;
     *         *total_weight += key[u];
     *     }
     *     
     *     // Update keys of adjacent vertices
     *     for (int v = 0; v < g->V; v++) {
     *         if (g->adj[u][v] && !in_mst[v] && g->adj[u][v] < key[v]) {
     *             key[v] = g->adj[u][v];
     *             parent[v] = u;
     *         }
     *     }
     * }
     */
    
    printf("[TODO] Prim's algorithm not implemented\n");
    
    /* Suppress unused variable warnings */
    (void)in_mst;
    (void)key;
    (void)parent;
}

/* =============================================================================
 * PART 4: TOPOLOGICAL SORT (DFS-BASED)
 * =============================================================================
 *
 * DFS-based topological sort:
 *   1. Perform DFS from each unvisited vertex
 *   2. Add vertex to result AFTER all descendants are processed
 *   3. Reverse the result (or use a stack)
 *
 * This produces reverse postorder traversal, which is a valid topological order
 */

/**
 * DFS helper for topological sort
 *
 * @param g       Pointer to graph
 * @param v       Current vertex
 * @param visited Array tracking visited vertices
 * @param result  Array to store topological order (filled from end)
 * @param idx     Pointer to current index in result (decreasing)
 *
 * TODO 5: Implement DFS for topological sort
 *         1. Mark v as visited
 *         2. Recursively visit all unvisited neighbours
 *         3. After all neighbours processed, add v to result
 *
 * HINT: Add vertex at result[(*idx)--] after processing all descendants
 */
void topo_dfs_visit(Graph *g, int v, bool visited[], int result[], int *idx) {
    /* TODO: Implement DFS visit for topological sort */
    
    visited[v] = true;
    
    /* Visit all neighbours */
    /* HINT:
     * for (int i = 0; i < g->adj_count[v]; i++) {
     *     int u = g->adj_list[v][i];
     *     if (!visited[u]) {
     *         topo_dfs_visit(g, u, visited, result, idx);
     *     }
     * }
     */
    
    /* Add vertex to result after all descendants processed */
    /* HINT: result[(*idx)--] = v; */
    
    /* Suppress unused variable warnings */
    (void)g;
    (void)result;
    (void)idx;
}

/**
 * Topological Sort using DFS (Reverse Postorder)
 *
 * @param g      Pointer to graph
 * @param result Array to store topological order
 */
void topological_sort_dfs(Graph *g, int result[]) {
    bool visited[MAX_VERTICES] = {false};
    int idx = g->V - 1;
    
    for (int v = 0; v < g->V; v++) {
        if (!visited[v]) {
            topo_dfs_visit(g, v, visited, result, &idx);
        }
    }
}

/* =============================================================================
 * PART 5: TOPOLOGICAL SORT (KAHN'S ALGORITHM)
 * =============================================================================
 *
 * Kahn's algorithm (BFS-based):
 *   1. Calculate in-degree of all vertices
 *   2. Add all vertices with in-degree 0 to queue
 *   3. Process queue:
 *      - Remove vertex, add to result
 *      - Decrease in-degree of neighbours
 *      - If neighbour's in-degree becomes 0, add to queue
 *   4. If result size != V, graph has a cycle
 */

/**
 * Topological Sort using Kahn's Algorithm (BFS)
 *
 * @param g      Pointer to graph
 * @param result Array to store topological order
 * @return       true if successful, false if cycle detected
 *
 * TODO 6: Implement Kahn's algorithm
 *         1. Calculate in-degree for all vertices
 *         2. Add vertices with in-degree 0 to queue
 *         3. Process queue: remove vertex, add to result,
 *            decrease in-degrees, add new zero in-degree vertices
 *         4. Check if all vertices processed (cycle detection)
 *
 * HINT: Use array as queue with front and rear pointers
 */
bool topological_sort_kahn(Graph *g, int result[]) {
    /* TODO: Implement Kahn's algorithm */
    
    int in_degree[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    int idx = 0;
    
    /* Step 1: Calculate in-degrees */
    /* HINT:
     * for (int u = 0; u < g->V; u++) {
     *     for (int i = 0; i < g->adj_count[u]; i++) {
     *         int v = g->adj_list[u][i];
     *         in_degree[v]++;
     *     }
     * }
     */
    
    /* Step 2: Enqueue vertices with in-degree 0 */
    /* HINT:
     * for (int v = 0; v < g->V; v++) {
     *     if (in_degree[v] == 0) {
     *         queue[rear++] = v;
     *     }
     * }
     */
    
    /* Step 3: Process queue */
    /* HINT:
     * while (front < rear) {
     *     int u = queue[front++];
     *     result[idx++] = u;
     *     
     *     for (int i = 0; i < g->adj_count[u]; i++) {
     *         int v = g->adj_list[u][i];
     *         in_degree[v]--;
     *         if (in_degree[v] == 0) {
     *             queue[rear++] = v;
     *         }
     *     }
     * }
     */
    
    /* Step 4: Check for cycle */
    /* HINT: return idx == g->V; */
    
    printf("[TODO] Kahn's algorithm not implemented\n");
    
    /* Suppress unused variable warnings */
    (void)in_degree;
    (void)queue;
    (void)front;
    (void)rear;
    (void)idx;
    (void)result;
    
    return false;
}

/* =============================================================================
 * TESTING AND DEMONSTRATION
 * =============================================================================
 */

void test_union_find(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Union-Find                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    UnionFind uf;
    uf_init(&uf, 10);
    
    uf_union(&uf, 0, 1);
    uf_union(&uf, 2, 3);
    uf_union(&uf, 0, 2);
    
    printf("After Union(0,1), Union(2,3), Union(0,2):\n");
    printf("  Connected(0, 3)? %s (expected: Yes)\n", 
           uf_connected(&uf, 0, 3) ? "Yes" : "No");
    printf("  Connected(0, 4)? %s (expected: No)\n", 
           uf_connected(&uf, 0, 4) ? "Yes" : "No");
}

void test_kruskal(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Kruskal's Algorithm                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Graph g;
    g.V = 4;
    g.E = 5;
    
    /* Edges: (src, dest, weight) */
    int edge_data[][3] = {{0, 1, 10}, {0, 2, 6}, {0, 3, 5}, {1, 3, 15}, {2, 3, 4}};
    for (int i = 0; i < g.E; i++) {
        g.edges[i].src = edge_data[i][0];
        g.edges[i].dest = edge_data[i][1];
        g.edges[i].weight = edge_data[i][2];
    }
    
    Edge mst[MAX_VERTICES];
    int mst_size, total_weight;
    
    kruskal_mst(&g, mst, &mst_size, &total_weight);
    
    printf("MST Edges:\n");
    for (int i = 0; i < mst_size; i++) {
        printf("  %d -- %d (weight: %d)\n", 
               mst[i].src, mst[i].dest, mst[i].weight);
    }
    printf("Total weight: %d (expected: 19)\n", total_weight);
}

void test_prim(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Prim's Algorithm                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Graph g;
    g.V = 4;
    memset(g.adj, 0, sizeof(g.adj));
    
    /* Same graph as Kruskal test */
    g.adj[0][1] = g.adj[1][0] = 10;
    g.adj[0][2] = g.adj[2][0] = 6;
    g.adj[0][3] = g.adj[3][0] = 5;
    g.adj[1][3] = g.adj[3][1] = 15;
    g.adj[2][3] = g.adj[3][2] = 4;
    
    Edge mst[MAX_VERTICES];
    int mst_size, total_weight;
    
    prim_mst(&g, 0, mst, &mst_size, &total_weight);
    
    printf("MST Edges:\n");
    for (int i = 0; i < mst_size; i++) {
        printf("  %d -- %d (weight: %d)\n", 
               mst[i].src, mst[i].dest, mst[i].weight);
    }
    printf("Total weight: %d (expected: 19)\n", total_weight);
}

void test_topological_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Topological Sort                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Graph g;
    g.V = 6;
    memset(g.adj_count, 0, sizeof(g.adj_count));
    
    /* DAG edges */
    int edges[][2] = {{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}};
    int E = 6;
    
    for (int i = 0; i < E; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        g.adj_list[u][g.adj_count[u]++] = v;
    }
    
    int result[MAX_VERTICES];
    
    printf("DFS-based topological sort:\n  ");
    topological_sort_dfs(&g, result);
    for (int i = 0; i < g.V; i++) {
        printf("%d ", result[i]);
    }
    printf("\n\n");
    
    printf("Kahn's algorithm:\n  ");
    if (topological_sort_kahn(&g, result)) {
        for (int i = 0; i < g.V; i++) {
            printf("%d ", result[i]);
        }
        printf("\n");
    } else {
        printf("Cycle detected or not implemented\n");
    }
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: EXERCISE 1                                       ║\n");
    printf("║     MST and Topological Sort                                  ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    test_union_find();
    test_kruskal();
    test_prim();
    test_topological_sort();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    EXERCISE COMPLETE                          ║\n");
    printf("║                                                               ║\n");
    printf("║  Implement the TODOs:                                         ║\n");
    printf("║    TODO 1: Union-Find with path compression                   ║\n");
    printf("║    TODO 2: Union by rank                                      ║\n");
    printf("║    TODO 3: Kruskal's algorithm                                ║\n");
    printf("║    TODO 4: Prim's algorithm                                   ║\n");
    printf("║    TODO 5: DFS topological sort                               ║\n");
    printf("║    TODO 6: Kahn's algorithm                                   ║\n");
    printf("║                                                               ║\n");
    printf("║  Run 'make test-ex1' to verify your implementation            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
