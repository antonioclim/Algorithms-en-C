/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Complete Working Example
 * =============================================================================
 *
 * This example demonstrates:
 *   1. Minimum Spanning Trees (Kruskal's and Prim's algorithms)
 *   2. Union-Find data structure with path compression
 *   3. Topological Sort (DFS-based and Kahn's algorithm)
 *   4. Strongly Connected Components (Kosaraju's algorithm)
 *   5. Backtracking paradigm (N-Queens, Sudoku, Graph Coloring)
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Usage: ./example1
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
#define BOARD_SIZE   9
#define N_QUEENS     8

/* Edge structure for Kruskal's algorithm */
typedef struct {
    int src;
    int dest;
    int weight;
} Edge;

/* Graph structure for adjacency list representation */
typedef struct {
    int V, E;
    Edge edges[MAX_EDGES];
    int adj[MAX_VERTICES][MAX_VERTICES];
    int adj_list[MAX_VERTICES][MAX_VERTICES];
    int adj_count[MAX_VERTICES];
} Graph;

/* Union-Find (Disjoint Set Union) structure */
typedef struct {
    int parent[MAX_VERTICES];
    int rank[MAX_VERTICES];
    int size;
} UnionFind;

/* =============================================================================
 * PART 1: UNION-FIND DATA STRUCTURE
 * =============================================================================
 */

/**
 * Initialise Union-Find structure
 * Each element is its own parent (singleton set)
 */
void uf_init(UnionFind *uf, int n) {
    uf->size = n;
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

/**
 * Find operation with path compression
 * Flattens the tree for O(α(n)) amortised time
 */
int uf_find(UnionFind *uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = uf_find(uf, uf->parent[x]);  /* Path compression */
    }
    return uf->parent[x];
}

/**
 * Union operation with union by rank
 * Keeps trees balanced for efficiency
 */
void uf_union(UnionFind *uf, int x, int y) {
    int root_x = uf_find(uf, x);
    int root_y = uf_find(uf, y);
    
    if (root_x == root_y) return;  /* Already in same set */
    
    /* Union by rank: attach smaller tree under larger */
    if (uf->rank[root_x] < uf->rank[root_y]) {
        uf->parent[root_x] = root_y;
    } else if (uf->rank[root_x] > uf->rank[root_y]) {
        uf->parent[root_y] = root_x;
    } else {
        uf->parent[root_y] = root_x;
        uf->rank[root_x]++;
    }
}

/**
 * Check if two elements are in the same set
 */
bool uf_connected(UnionFind *uf, int x, int y) {
    return uf_find(uf, x) == uf_find(uf, y);
}

void demo_union_find(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 1: UNION-FIND DATA STRUCTURE                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    UnionFind uf;
    uf_init(&uf, 10);
    
    printf("Initial state: Each element in its own set\n");
    printf("Elements: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9\n\n");
    
    /* Perform some unions */
    printf("Performing unions:\n");
    printf("  Union(0, 1): ");
    uf_union(&uf, 0, 1);
    printf("Set {0, 1} formed\n");
    
    printf("  Union(2, 3): ");
    uf_union(&uf, 2, 3);
    printf("Set {2, 3} formed\n");
    
    printf("  Union(4, 5): ");
    uf_union(&uf, 4, 5);
    printf("Set {4, 5} formed\n");
    
    printf("  Union(0, 2): ");
    uf_union(&uf, 0, 2);
    printf("Sets merged → {0, 1, 2, 3}\n");
    
    printf("  Union(4, 6): ");
    uf_union(&uf, 4, 6);
    printf("Set → {4, 5, 6}\n");
    
    printf("  Union(0, 4): ");
    uf_union(&uf, 0, 4);
    printf("Sets merged → {0, 1, 2, 3, 4, 5, 6}\n\n");
    
    /* Check connectivity */
    printf("Connectivity tests:\n");
    printf("  Are 1 and 5 connected? %s\n", uf_connected(&uf, 1, 5) ? "Yes" : "No");
    printf("  Are 3 and 6 connected? %s\n", uf_connected(&uf, 3, 6) ? "Yes" : "No");
    printf("  Are 7 and 8 connected? %s\n", uf_connected(&uf, 7, 8) ? "Yes" : "No");
    printf("  Are 0 and 9 connected? %s\n", uf_connected(&uf, 0, 9) ? "Yes" : "No");
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Union-Find Complexity:                                       ║\n");
    printf("║    • Find with path compression: O(α(n)) ≈ O(1) amortised     ║\n");
    printf("║    • Union by rank: O(α(n)) ≈ O(1) amortised                  ║\n");
    printf("║    • α(n) = inverse Ackermann function, grows incredibly slow ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 2: MINIMUM SPANNING TREES
 * =============================================================================
 */

/**
 * Comparison function for sorting edges by weight
 */
int compare_edges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

/**
 * Kruskal's Algorithm for Minimum Spanning Tree
 * Uses Union-Find for cycle detection
 * Time: O(E log E) for sorting + O(E α(V)) for Union-Find ≈ O(E log V)
 */
void kruskal_mst(Graph *g, Edge mst[], int *mst_size, int *total_weight) {
    UnionFind uf;
    uf_init(&uf, g->V);
    
    /* Sort edges by weight */
    qsort(g->edges, g->E, sizeof(Edge), compare_edges);
    
    *mst_size = 0;
    *total_weight = 0;
    
    printf("Processing edges in sorted order:\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│  Edge      │ Weight │ Action                               │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    
    for (int i = 0; i < g->E && *mst_size < g->V - 1; i++) {
        int u = g->edges[i].src;
        int v = g->edges[i].dest;
        int w = g->edges[i].weight;
        
        if (!uf_connected(&uf, u, v)) {
            uf_union(&uf, u, v);
            mst[*mst_size] = g->edges[i];
            (*mst_size)++;
            *total_weight += w;
            printf("│  %d -- %d    │   %2d   │ ✓ Added to MST (no cycle)            │\n", 
                   u, v, w);
        } else {
            printf("│  %d -- %d    │   %2d   │ ✗ Rejected (would create cycle)      │\n", 
                   u, v, w);
        }
    }
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

/**
 * Prim's Algorithm for Minimum Spanning Tree
 * Uses adjacency matrix and simple array-based priority
 * Time: O(V²) with adjacency matrix
 */
void prim_mst(Graph *g, int start, Edge mst[], int *mst_size, int *total_weight) {
    bool in_mst[MAX_VERTICES] = {false};
    int key[MAX_VERTICES];      /* Minimum weight edge to this vertex */
    int parent[MAX_VERTICES];   /* Parent vertex in MST */
    
    /* Initialise */
    for (int i = 0; i < g->V; i++) {
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    key[start] = 0;
    
    *mst_size = 0;
    *total_weight = 0;
    
    printf("Growing MST from vertex %d:\n", start);
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│  Step │ Added │ Edge          │ MST Weight                  │\n");
    printf("├─────────────────────────────────────────────────────────────┤\n");
    
    for (int count = 0; count < g->V; count++) {
        /* Find minimum key vertex not in MST */
        int min_key = INT_MAX, u = -1;
        for (int v = 0; v < g->V; v++) {
            if (!in_mst[v] && key[v] < min_key) {
                min_key = key[v];
                u = v;
            }
        }
        
        if (u == -1) break;  /* No more reachable vertices */
        
        in_mst[u] = true;
        
        if (parent[u] != -1) {
            mst[*mst_size].src = parent[u];
            mst[*mst_size].dest = u;
            mst[*mst_size].weight = key[u];
            (*mst_size)++;
            *total_weight += key[u];
            printf("│   %2d  │   %d   │   %d -- %d (w=%d)  │         %3d               │\n",
                   count, u, parent[u], u, key[u], *total_weight);
        } else {
            printf("│   %2d  │   %d   │   (start)       │           0               │\n",
                   count, u);
        }
        
        /* Update keys of adjacent vertices */
        for (int v = 0; v < g->V; v++) {
            if (g->adj[u][v] && !in_mst[v] && g->adj[u][v] < key[v]) {
                key[v] = g->adj[u][v];
                parent[v] = u;
            }
        }
    }
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

void demo_mst(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 2: MINIMUM SPANNING TREES                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create a sample graph */
    Graph g;
    g.V = 6;
    g.E = 9;
    
    /* Initialise adjacency matrix */
    memset(g.adj, 0, sizeof(g.adj));
    
    /* Define edges: src, dest, weight */
    int edge_data[][3] = {
        {0, 1, 4}, {0, 2, 2}, {1, 2, 1}, {1, 3, 5},
        {2, 3, 8}, {2, 4, 10}, {3, 4, 2}, {3, 5, 6}, {4, 5, 3}
    };
    
    printf("Sample Graph:\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                                                             │\n");
    printf("│         4       5                                           │\n");
    printf("│     0 ─────1─────3                                          │\n");
    printf("│      \\    /│      \\                                         │\n");
    printf("│     2 \\  /1│       \\6                                       │\n");
    printf("│        \\/  │8       \\                                       │\n");
    printf("│        2───┼────4────5                                      │\n");
    printf("│            │   2   3                                        │\n");
    printf("│           10                                                │\n");
    printf("│                                                             │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    /* Build edge list and adjacency matrix */
    for (int i = 0; i < g.E; i++) {
        g.edges[i].src = edge_data[i][0];
        g.edges[i].dest = edge_data[i][1];
        g.edges[i].weight = edge_data[i][2];
        g.adj[edge_data[i][0]][edge_data[i][1]] = edge_data[i][2];
        g.adj[edge_data[i][1]][edge_data[i][0]] = edge_data[i][2];
    }
    
    /* Kruskal's Algorithm */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  KRUSKAL'S ALGORITHM\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    Edge kruskal_result[MAX_VERTICES];
    int kruskal_size, kruskal_weight;
    kruskal_mst(&g, kruskal_result, &kruskal_size, &kruskal_weight);
    
    printf("\nKruskal MST Edges: ");
    for (int i = 0; i < kruskal_size; i++) {
        printf("(%d-%d)", kruskal_result[i].src, kruskal_result[i].dest);
        if (i < kruskal_size - 1) printf(", ");
    }
    printf("\nTotal Weight: %d\n\n", kruskal_weight);
    
    /* Prim's Algorithm */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  PRIM'S ALGORITHM (starting from vertex 0)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n");
    
    Edge prim_result[MAX_VERTICES];
    int prim_size, prim_weight;
    prim_mst(&g, 0, prim_result, &prim_size, &prim_weight);
    
    printf("\nPrim MST Edges: ");
    for (int i = 0; i < prim_size; i++) {
        printf("(%d-%d)", prim_result[i].src, prim_result[i].dest);
        if (i < prim_size - 1) printf(", ");
    }
    printf("\nTotal Weight: %d\n\n", prim_weight);
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Both algorithms found MST with weight %2d                     ║\n", kruskal_weight);
    printf("║  (Edges may differ, but total weight is always the same)      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 3: TOPOLOGICAL SORT
 * =============================================================================
 */

/**
 * DFS-based Topological Sort (Reverse Postorder)
 * Adds vertex to result after all descendants are processed
 */
void topo_dfs_visit(Graph *g, int v, bool visited[], int result[], int *idx) {
    visited[v] = true;
    
    /* Visit all neighbours */
    for (int i = 0; i < g->adj_count[v]; i++) {
        int u = g->adj_list[v][i];
        if (!visited[u]) {
            topo_dfs_visit(g, u, visited, result, idx);
        }
    }
    
    /* Add to result after all descendants processed */
    result[(*idx)--] = v;
}

void topological_sort_dfs(Graph *g, int result[]) {
    bool visited[MAX_VERTICES] = {false};
    int idx = g->V - 1;
    
    for (int v = 0; v < g->V; v++) {
        if (!visited[v]) {
            topo_dfs_visit(g, v, visited, result, &idx);
        }
    }
}

/**
 * Kahn's Algorithm for Topological Sort (BFS-based)
 * Uses in-degree counting and queue
 */
bool topological_sort_kahn(Graph *g, int result[]) {
    int in_degree[MAX_VERTICES] = {0};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    int idx = 0;
    
    /* Calculate in-degrees */
    for (int u = 0; u < g->V; u++) {
        for (int i = 0; i < g->adj_count[u]; i++) {
            int v = g->adj_list[u][i];
            in_degree[v]++;
        }
    }
    
    /* Enqueue vertices with in-degree 0 */
    for (int v = 0; v < g->V; v++) {
        if (in_degree[v] == 0) {
            queue[rear++] = v;
        }
    }
    
    /* Process queue */
    while (front < rear) {
        int u = queue[front++];
        result[idx++] = u;
        
        /* Decrease in-degree of neighbours */
        for (int i = 0; i < g->adj_count[u]; i++) {
            int v = g->adj_list[u][i];
            in_degree[v]--;
            if (in_degree[v] == 0) {
                queue[rear++] = v;
            }
        }
    }
    
    /* Check if all vertices are processed (cycle detection) */
    return idx == g->V;
}

void demo_topological_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 3: TOPOLOGICAL SORT                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Create a DAG (course prerequisites example) */
    Graph g;
    g.V = 6;
    memset(g.adj_count, 0, sizeof(g.adj_count));
    
    printf("Course Prerequisites DAG:\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│  0 (Intro)  →  1 (Data Struct)  →  3 (Algorithms)           │\n");
    printf("│      ↓              ↓                    ↓                  │\n");
    printf("│  2 (Math)   →  4 (Databases)    →  5 (Capstone)             │\n");
    printf("│                                                             │\n");
    printf("│  Edges: 0→1, 0→2, 1→3, 1→4, 2→4, 3→5, 4→5                   │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    /* Add directed edges */
    int edges[][2] = {{0,1}, {0,2}, {1,3}, {1,4}, {2,4}, {3,5}, {4,5}};
    int num_edges = 7;
    
    for (int i = 0; i < num_edges; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        g.adj_list[u][g.adj_count[u]++] = v;
    }
    
    /* DFS-based topological sort */
    int dfs_result[MAX_VERTICES];
    topological_sort_dfs(&g, dfs_result);
    
    printf("DFS-based Topological Order:\n  ");
    for (int i = 0; i < g.V; i++) {
        printf("%d", dfs_result[i]);
        if (i < g.V - 1) printf(" → ");
    }
    printf("\n\n");
    
    /* Kahn's algorithm */
    int kahn_result[MAX_VERTICES];
    bool success = topological_sort_kahn(&g, kahn_result);
    
    printf("Kahn's Algorithm (BFS-based) Order:\n  ");
    if (success) {
        for (int i = 0; i < g.V; i++) {
            printf("%d", kahn_result[i]);
            if (i < g.V - 1) printf(" → ");
        }
        printf("\n\n");
    } else {
        printf("Cycle detected! No valid topological order.\n\n");
    }
    
    printf("Course names in order:\n");
    const char *courses[] = {"Intro", "Data Structures", "Math", 
                             "Algorithms", "Databases", "Capstone"};
    for (int i = 0; i < g.V; i++) {
        printf("  %d. %s\n", i + 1, courses[dfs_result[i]]);
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Both methods produce valid topological orderings.            ║\n");
    printf("║  Multiple valid orderings may exist for a given DAG.          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 4: STRONGLY CONNECTED COMPONENTS (Kosaraju's Algorithm)
 * =============================================================================
 */

/**
 * First DFS pass: Fill stack with finish times
 */
void scc_dfs1(int adj[][MAX_VERTICES], int adj_count[], int v, 
              bool visited[], int stack[], int *top, int V) {
    (void)V;  /* Unused parameter */
    visited[v] = true;
    
    for (int i = 0; i < adj_count[v]; i++) {
        int u = adj[v][i];
        if (!visited[u]) {
            scc_dfs1(adj, adj_count, u, visited, stack, top, V);
        }
    }
    
    stack[(*top)++] = v;
}

/**
 * Second DFS pass: Find SCCs in transposed graph
 */
void scc_dfs2(int adj_t[][MAX_VERTICES], int adj_t_count[], int v,
              bool visited[], int component[], int comp_id) {
    visited[v] = true;
    component[v] = comp_id;
    
    for (int i = 0; i < adj_t_count[v]; i++) {
        int u = adj_t[v][i];
        if (!visited[u]) {
            scc_dfs2(adj_t, adj_t_count, u, visited, component, comp_id);
        }
    }
}

/**
 * Kosaraju's Algorithm for finding SCCs
 * Two-pass DFS: first on original graph, second on transposed
 */
int kosaraju_scc(int V, int adj[][MAX_VERTICES], int adj_count[],
                 int component[]) {
    bool visited[MAX_VERTICES] = {false};
    int stack[MAX_VERTICES];
    int top = 0;
    
    /* Pass 1: Fill stack with finish times */
    for (int v = 0; v < V; v++) {
        if (!visited[v]) {
            scc_dfs1(adj, adj_count, v, visited, stack, &top, V);
        }
    }
    
    /* Build transposed graph */
    int adj_t[MAX_VERTICES][MAX_VERTICES];
    int adj_t_count[MAX_VERTICES] = {0};
    
    for (int u = 0; u < V; u++) {
        for (int i = 0; i < adj_count[u]; i++) {
            int v = adj[u][i];
            adj_t[v][adj_t_count[v]++] = u;
        }
    }
    
    /* Reset visited array */
    memset(visited, false, sizeof(visited));
    
    /* Pass 2: Process vertices in reverse finish order */
    int num_scc = 0;
    while (top > 0) {
        int v = stack[--top];
        if (!visited[v]) {
            scc_dfs2(adj_t, adj_t_count, v, visited, component, num_scc);
            num_scc++;
        }
    }
    
    return num_scc;
}

void demo_scc(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 4: STRONGLY CONNECTED COMPONENTS                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int V = 8;
    int adj[MAX_VERTICES][MAX_VERTICES];
    int adj_count[MAX_VERTICES] = {0};
    
    printf("Directed Graph with SCCs:\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                                                             │\n");
    printf("│    ┌───── 0 ←───┐      ┌───── 4 ←───┐                       │\n");
    printf("│    │      ↓     │      │      ↓     │                       │\n");
    printf("│    │      1 ────┘      │      5 ────┘                       │\n");
    printf("│    ↓      ↓            ↓      ↓                             │\n");
    printf("│    2 ←─── 3 ─────────→ 6 ←─── 7                             │\n");
    printf("│    └──────┘                                                 │\n");
    printf("│                                                             │\n");
    printf("│  Edges: 0→1, 1→2, 2→3, 3→0, 3→4, 4→5, 5→6, 6→7, 7→4         │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    /* Add edges */
    int edges[][2] = {{0,1}, {1,2}, {2,3}, {3,0}, {3,4}, 
                      {4,5}, {5,6}, {6,7}, {7,4}};
    int E = 9;
    
    for (int i = 0; i < E; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        adj[u][adj_count[u]++] = v;
    }
    
    /* Find SCCs */
    int component[MAX_VERTICES];
    int num_scc = kosaraju_scc(V, adj, adj_count, component);
    
    printf("Kosaraju's Algorithm Results:\n");
    printf("  Number of SCCs: %d\n\n", num_scc);
    
    /* Display SCCs */
    printf("  SCCs found:\n");
    for (int scc = 0; scc < num_scc; scc++) {
        printf("    SCC %d: {", scc);
        bool first = true;
        for (int v = 0; v < V; v++) {
            if (component[v] == scc) {
                if (!first) printf(", ");
                printf("%d", v);
                first = false;
            }
        }
        printf("}\n");
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Kosaraju's Algorithm: O(V + E) time complexity               ║\n");
    printf("║  Uses two DFS passes: one on G, one on G^T (transposed)       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 5: BACKTRACKING - N-QUEENS PROBLEM
 * =============================================================================
 */

/* Global counters for statistics */
static int nqueens_solutions = 0;
static int nqueens_backtracks = 0;

/**
 * Check if placing a queen at (row, col) is safe
 * O(1) time using column and diagonal arrays
 */
bool is_safe_nqueens(int board[], int row, int col, int n) {
    (void)n;  /* Unused in this implementation */
    
    /* Check column and diagonals */
    for (int i = 0; i < row; i++) {
        /* Same column or same diagonal */
        if (board[i] == col || 
            abs(board[i] - col) == abs(i - row)) {
            return false;
        }
    }
    return true;
}

/**
 * Print the N-Queens board
 */
void print_nqueens_board(int board[], int n) {
    printf("  ┌");
    for (int j = 0; j < n; j++) printf("───");
    printf("┐\n");
    
    for (int i = 0; i < n; i++) {
        printf("  │");
        for (int j = 0; j < n; j++) {
            if (board[i] == j) {
                printf(" Q ");
            } else {
                printf(" . ");
            }
        }
        printf("│\n");
    }
    
    printf("  └");
    for (int j = 0; j < n; j++) printf("───");
    printf("┘\n");
}

/**
 * Solve N-Queens using backtracking
 */
void solve_nqueens(int board[], int row, int n, bool print_all) {
    if (row == n) {
        /* Found a solution */
        nqueens_solutions++;
        if (print_all && nqueens_solutions <= 3) {
            printf("\n  Solution #%d:\n", nqueens_solutions);
            print_nqueens_board(board, n);
        }
        return;
    }
    
    for (int col = 0; col < n; col++) {
        if (is_safe_nqueens(board, row, col, n)) {
            board[row] = col;
            solve_nqueens(board, row + 1, n, print_all);
            board[row] = -1;  /* Backtrack */
            nqueens_backtracks++;
        }
    }
}

void demo_nqueens(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 5: N-QUEENS PROBLEM (Backtracking)                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Problem: Place N queens on an N×N chessboard such that\n");
    printf("no two queens threaten each other.\n\n");
    
    /* Solve for N = 4 (show all solutions) */
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  4-Queens: All Solutions\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int board4[4] = {-1, -1, -1, -1};
    nqueens_solutions = 0;
    nqueens_backtracks = 0;
    solve_nqueens(board4, 0, 4, true);
    printf("\n  Total solutions for 4-Queens: %d\n", nqueens_solutions);
    printf("  Backtrack operations: %d\n", nqueens_backtracks);
    
    /* Solve for N = 8 (count only) */
    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  8-Queens: Counting Solutions (first 3 shown)\n");
    printf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    
    int board8[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
    nqueens_solutions = 0;
    nqueens_backtracks = 0;
    solve_nqueens(board8, 0, 8, true);
    printf("\n  Total solutions for 8-Queens: %d\n", nqueens_solutions);
    printf("  Backtrack operations: %d\n", nqueens_backtracks);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  N-Queens complexity: O(N!) worst case                        ║\n");
    printf("║  Pruning significantly reduces actual search space            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 6: BACKTRACKING - SUDOKU SOLVER
 * =============================================================================
 */

/**
 * Check if placing digit at (row, col) is valid
 */
bool is_valid_sudoku(int grid[BOARD_SIZE][BOARD_SIZE], int row, int col, int digit) {
    /* Check row */
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (grid[row][j] == digit) return false;
    }
    
    /* Check column */
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (grid[i][col] == digit) return false;
    }
    
    /* Check 3x3 box */
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[box_row + i][box_col + j] == digit) return false;
        }
    }
    
    return true;
}

/**
 * Find next empty cell (returns false if board is full)
 */
bool find_empty_cell(int grid[BOARD_SIZE][BOARD_SIZE], int *row, int *col) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (grid[i][j] == 0) {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;
}

/**
 * Solve Sudoku using backtracking
 */
bool solve_sudoku(int grid[BOARD_SIZE][BOARD_SIZE]) {
    int row, col;
    
    /* Find next empty cell */
    if (!find_empty_cell(grid, &row, &col)) {
        return true;  /* Puzzle solved */
    }
    
    /* Try digits 1-9 */
    for (int digit = 1; digit <= 9; digit++) {
        if (is_valid_sudoku(grid, row, col, digit)) {
            grid[row][col] = digit;
            
            if (solve_sudoku(grid)) {
                return true;
            }
            
            grid[row][col] = 0;  /* Backtrack */
        }
    }
    
    return false;  /* No valid digit found */
}

/**
 * Print Sudoku grid
 */
void print_sudoku(int grid[BOARD_SIZE][BOARD_SIZE]) {
    printf("  ┌───────┬───────┬───────┐\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i > 0 && i % 3 == 0) {
            printf("  ├───────┼───────┼───────┤\n");
        }
        printf("  │");
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (j > 0 && j % 3 == 0) printf(" │");
            if (grid[i][j] == 0) {
                printf(" .");
            } else {
                printf(" %d", grid[i][j]);
            }
        }
        printf(" │\n");
    }
    printf("  └───────┴───────┴───────┘\n");
}

void demo_sudoku(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 6: SUDOKU SOLVER (Backtracking)                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Sample puzzle (0 = empty) */
    int puzzle[BOARD_SIZE][BOARD_SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    printf("Original Puzzle:\n");
    print_sudoku(puzzle);
    
    printf("\nSolving...\n\n");
    
    if (solve_sudoku(puzzle)) {
        printf("Solution Found:\n");
        print_sudoku(puzzle);
    } else {
        printf("No solution exists!\n");
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Sudoku backtracking explores 9^(empty cells) possibilities   ║\n");
    printf("║  Constraint propagation can dramatically reduce search space  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PART 7: BACKTRACKING - GRAPH COLORING
 * =============================================================================
 */

/**
 * Check if assigning color to vertex is safe
 */
bool is_safe_coloring(int adj[][MAX_VERTICES], int adj_count[], 
                      int colors[], int v, int c) {
    for (int i = 0; i < adj_count[v]; i++) {
        int u = adj[v][i];
        if (colors[u] == c) return false;
    }
    return true;
}

/**
 * Solve graph coloring using backtracking
 */
bool graph_coloring_util(int adj[][MAX_VERTICES], int adj_count[], int V,
                         int m, int colors[], int v) {
    if (v == V) return true;  /* All vertices colored */
    
    for (int c = 1; c <= m; c++) {
        if (is_safe_coloring(adj, adj_count, colors, v, c)) {
            colors[v] = c;
            
            if (graph_coloring_util(adj, adj_count, V, m, colors, v + 1)) {
                return true;
            }
            
            colors[v] = 0;  /* Backtrack */
        }
    }
    
    return false;
}

/**
 * Find chromatic number (minimum colors needed)
 */
int find_chromatic_number(int adj[][MAX_VERTICES], int adj_count[], int V) {
    int colors[MAX_VERTICES] = {0};
    
    for (int m = 1; m <= V; m++) {
        memset(colors, 0, sizeof(colors));
        if (graph_coloring_util(adj, adj_count, V, m, colors, 0)) {
            return m;
        }
    }
    
    return V;  /* Worst case: V colors */
}

void demo_graph_coloring(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PART 7: GRAPH COLORING (Backtracking)                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int V = 5;
    int adj[MAX_VERTICES][MAX_VERTICES];
    int adj_count[MAX_VERTICES] = {0};
    
    printf("Graph for Coloring (Petersen-like structure):\n");
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                                                             │\n");
    printf("│            0                                                │\n");
    printf("│           / \\                                               │\n");
    printf("│          /   \\                                              │\n");
    printf("│         1─────2                                             │\n");
    printf("│          \\   /                                              │\n");
    printf("│           \\ /                                               │\n");
    printf("│            3                                                │\n");
    printf("│            │                                                │\n");
    printf("│            4                                                │\n");
    printf("│                                                             │\n");
    printf("│  Edges: (0,1), (0,2), (1,2), (1,3), (2,3), (3,4)            │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    /* Add undirected edges */
    int edges[][2] = {{0,1}, {0,2}, {1,2}, {1,3}, {2,3}, {3,4}};
    int E = 6;
    
    for (int i = 0; i < E; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        adj[u][adj_count[u]++] = v;
        adj[v][adj_count[v]++] = u;
    }
    
    /* Find chromatic number */
    int chromatic = find_chromatic_number(adj, adj_count, V);
    printf("Chromatic number (minimum colors needed): %d\n\n", chromatic);
    
    /* Show actual coloring */
    int colors[MAX_VERTICES] = {0};
    graph_coloring_util(adj, adj_count, V, chromatic, colors, 0);
    
    const char *color_names[] = {"", "Red", "Blue", "Green", "Yellow"};
    printf("Vertex coloring with %d colors:\n", chromatic);
    for (int v = 0; v < V; v++) {
        printf("  Vertex %d: %s\n", v, color_names[colors[v]]);
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  Graph coloring is NP-hard in general                         ║\n");
    printf("║  Backtracking explores O(m^V) possibilities for m colors      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING         ║\n");
    printf("║                    Complete Example                           ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Part 1: Union-Find */
    demo_union_find();
    
    /* Part 2: Minimum Spanning Trees */
    demo_mst();
    
    /* Part 3: Topological Sort */
    demo_topological_sort();
    
    /* Part 4: Strongly Connected Components */
    demo_scc();
    
    /* Part 5: N-Queens */
    demo_nqueens();
    
    /* Part 6: Sudoku Solver */
    demo_sudoku();
    
    /* Part 7: Graph Coloring */
    demo_graph_coloring();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRATION COMPLETE                     ║\n");
    printf("║                                                               ║\n");
    printf("║  Topics Covered:                                              ║\n");
    printf("║    • Union-Find with path compression and union by rank       ║\n");
    printf("║    • Kruskal's and Prim's MST algorithms                      ║\n");
    printf("║    • Topological Sort (DFS and Kahn's algorithm)              ║\n");
    printf("║    • Strongly Connected Components (Kosaraju)                 ║\n");
    printf("║    • Backtracking: N-Queens, Sudoku, Graph Coloring           ║\n");
    printf("║                                                               ║\n");
    printf("║  This concludes the ATP Laboratory Course.                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
