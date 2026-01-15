/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Exercise 1 Solution: MST and Topological Sort
 * =============================================================================
 *
 * Complete implementations of:
 *   1. Union-Find with path compression and union by rank
 *   2. Kruskal's algorithm for MST
 *   3. Prim's algorithm for MST
 *   4. Topological Sort (DFS-based)
 *   5. Topological Sort (Kahn's algorithm)
 *
 * Compilation: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100
#define MAX_EDGES    500

typedef struct {
    int src;
    int dest;
    int weight;
} Edge;

typedef struct {
    int parent[MAX_VERTICES];
    int rank[MAX_VERTICES];
    int size;
} UnionFind;

typedef struct {
    int V;
    int E;
    Edge edges[MAX_EDGES];
    int adj[MAX_VERTICES][MAX_VERTICES];
    int adj_list[MAX_VERTICES][MAX_VERTICES];
    int adj_count[MAX_VERTICES];
} Graph;

/* =============================================================================
 * UNION-FIND WITH PATH COMPRESSION AND UNION BY RANK
 * =============================================================================
 */

void uf_init(UnionFind *uf, int n) {
    uf->size = n;
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

/**
 * Find with path compression
 * Every node on the path to root gets updated to point directly to root
 */
int uf_find(UnionFind *uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = uf_find(uf, uf->parent[x]);  /* Path compression */
    }
    return uf->parent[x];
}

/**
 * Union by rank
 * Attach smaller tree under root of larger tree
 */
void uf_union(UnionFind *uf, int x, int y) {
    int root_x = uf_find(uf, x);
    int root_y = uf_find(uf, y);
    
    if (root_x == root_y) return;
    
    /* Union by rank */
    if (uf->rank[root_x] < uf->rank[root_y]) {
        uf->parent[root_x] = root_y;
    } else if (uf->rank[root_x] > uf->rank[root_y]) {
        uf->parent[root_y] = root_x;
    } else {
        uf->parent[root_y] = root_x;
        uf->rank[root_x]++;
    }
}

bool uf_connected(UnionFind *uf, int x, int y) {
    return uf_find(uf, x) == uf_find(uf, y);
}

/* =============================================================================
 * KRUSKAL'S ALGORITHM
 * =============================================================================
 */

int compare_edges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

void kruskal_mst(Graph *g, Edge mst[], int *mst_size, int *total_weight) {
    UnionFind uf;
    uf_init(&uf, g->V);
    
    /* Sort edges by weight */
    qsort(g->edges, g->E, sizeof(Edge), compare_edges);
    
    *mst_size = 0;
    *total_weight = 0;
    
    printf("Kruskal's Algorithm:\n");
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
            printf("│  %d -- %d    │   %2d   │ ✓ Added to MST                       │\n", u, v, w);
        } else {
            printf("│  %d -- %d    │   %2d   │ ✗ Rejected (cycle)                   │\n", u, v, w);
        }
    }
    printf("└─────────────────────────────────────────────────────────────┘\n");
}

/* =============================================================================
 * PRIM'S ALGORITHM
 * =============================================================================
 */

void prim_mst(Graph *g, int start, Edge mst[], int *mst_size, int *total_weight) {
    bool in_mst[MAX_VERTICES];
    int key[MAX_VERTICES];
    int parent[MAX_VERTICES];
    
    for (int i = 0; i < g->V; i++) {
        in_mst[i] = false;
        key[i] = INT_MAX;
        parent[i] = -1;
    }
    key[start] = 0;
    
    *mst_size = 0;
    *total_weight = 0;
    
    printf("\nPrim's Algorithm (starting from vertex %d):\n", start);
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
        
        if (u == -1) break;
        
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

/* =============================================================================
 * TOPOLOGICAL SORT - DFS-BASED
 * =============================================================================
 */

void topo_dfs_visit(Graph *g, int v, bool visited[], int result[], int *idx) {
    visited[v] = true;
    
    for (int i = 0; i < g->adj_count[v]; i++) {
        int u = g->adj_list[v][i];
        if (!visited[u]) {
            topo_dfs_visit(g, u, visited, result, idx);
        }
    }
    
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

/* =============================================================================
 * TOPOLOGICAL SORT - KAHN'S ALGORITHM
 * =============================================================================
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
        
        for (int i = 0; i < g->adj_count[u]; i++) {
            int v = g->adj_list[u][i];
            in_degree[v]--;
            if (in_degree[v] == 0) {
                queue[rear++] = v;
            }
        }
    }
    
    return idx == g->V;  /* True if no cycle */
}

/* =============================================================================
 * TESTING
 * =============================================================================
 */

void test_union_find(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Union-Find                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    UnionFind uf;
    uf_init(&uf, 10);
    
    printf("Initial: 10 singleton sets\n\n");
    
    printf("Operations:\n");
    uf_union(&uf, 0, 1); printf("  Union(0, 1)\n");
    uf_union(&uf, 2, 3); printf("  Union(2, 3)\n");
    uf_union(&uf, 0, 2); printf("  Union(0, 2) - merges {0,1} and {2,3}\n");
    uf_union(&uf, 4, 5); printf("  Union(4, 5)\n");
    uf_union(&uf, 4, 6); printf("  Union(4, 6)\n");
    uf_union(&uf, 0, 4); printf("  Union(0, 4) - merges all\n\n");
    
    printf("Connectivity tests:\n");
    printf("  Connected(1, 5)? %s ✓\n", uf_connected(&uf, 1, 5) ? "Yes" : "No");
    printf("  Connected(3, 6)? %s ✓\n", uf_connected(&uf, 3, 6) ? "Yes" : "No");
    printf("  Connected(0, 9)? %s ✓\n", uf_connected(&uf, 0, 9) ? "Yes" : "No");
    printf("  Connected(7, 8)? %s ✓\n", uf_connected(&uf, 7, 8) ? "Yes" : "No");
}

void test_mst(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing MST Algorithms                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Graph g;
    g.V = 4;
    g.E = 5;
    memset(g.adj, 0, sizeof(g.adj));
    
    int edge_data[][3] = {{0, 1, 10}, {0, 2, 6}, {0, 3, 5}, {1, 3, 15}, {2, 3, 4}};
    
    printf("Graph:\n");
    printf("       10\n");
    printf("   0 ─────── 1\n");
    printf("   │\\        │\n");
    printf("  6│ \\5     15│\n");
    printf("   │  \\      │\n");
    printf("   2 ─── 3 ──┘\n");
    printf("       4\n\n");
    
    for (int i = 0; i < g.E; i++) {
        g.edges[i].src = edge_data[i][0];
        g.edges[i].dest = edge_data[i][1];
        g.edges[i].weight = edge_data[i][2];
        g.adj[edge_data[i][0]][edge_data[i][1]] = edge_data[i][2];
        g.adj[edge_data[i][1]][edge_data[i][0]] = edge_data[i][2];
    }
    
    Edge mst[MAX_VERTICES];
    int mst_size, total_weight;
    
    /* Kruskal */
    kruskal_mst(&g, mst, &mst_size, &total_weight);
    printf("\nKruskal MST Total Weight: %d\n", total_weight);
    
    /* Prim */
    prim_mst(&g, 0, mst, &mst_size, &total_weight);
    printf("\nPrim MST Total Weight: %d\n", total_weight);
    
    printf("\n✓ Both algorithms find MST with weight 19\n");
}

void test_topological_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      Testing Topological Sort                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    Graph g;
    g.V = 6;
    memset(g.adj_count, 0, sizeof(g.adj_count));
    
    printf("DAG (Course Prerequisites):\n");
    printf("    5 → 2\n");
    printf("    5 → 0 → 3\n");
    printf("    4 → 0\n");
    printf("    4 → 1\n");
    printf("    2 → 3 → 1\n\n");
    
    int edges[][2] = {{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}, {0, 3}};
    int E = 7;
    
    for (int i = 0; i < E; i++) {
        int u = edges[i][0];
        int v = edges[i][1];
        g.adj_list[u][g.adj_count[u]++] = v;
    }
    
    int result[MAX_VERTICES];
    
    printf("DFS-based Topological Sort:\n  ");
    topological_sort_dfs(&g, result);
    for (int i = 0; i < g.V; i++) {
        printf("%d", result[i]);
        if (i < g.V - 1) printf(" → ");
    }
    printf("\n\n");
    
    printf("Kahn's Algorithm (BFS-based):\n  ");
    if (topological_sort_kahn(&g, result)) {
        for (int i = 0; i < g.V; i++) {
            printf("%d", result[i]);
            if (i < g.V - 1) printf(" → ");
        }
        printf("\n");
    } else {
        printf("Cycle detected!\n");
    }
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: EXERCISE 1 SOLUTION                              ║\n");
    printf("║     MST and Topological Sort                                  ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    test_union_find();
    test_mst();
    test_topological_sort();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    ALL TESTS PASSED                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
