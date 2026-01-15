/**
 * =============================================================================
 * WEEK 16: ADVANCED GRAPH ALGORITHMS & BACKTRACKING
 * Homework 1 Solution - Graph Algorithms Suite
 * =============================================================================
 *
 * This solution implements:
 *   Part A: Minimum Cost Network (Kruskal's MST with city names)
 *   Part B: Course Prerequisite Scheduler (Topological Sort)
 *   Part C: Network Reliability Analysis (Articulation Points & Bridges)
 *
 * Usage:
 *   ./homework1 mst <input_file>
 *   ./homework1 topo <input_file>
 *   ./homework1 reliability <input_file>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_VERTICES 100
#define MAX_NAME_LEN 64
#define MAX_EDGES 1000

/* =============================================================================
 * PART A: MINIMUM COST NETWORK
 * =============================================================================
 * Uses Kruskal's algorithm with Union-Find to find MST for city connections.
 */

typedef struct {
    int src, dest;
    int weight;
} Edge;

typedef struct {
    int parent[MAX_VERTICES];
    int rank[MAX_VERTICES];
    int size;
} UnionFind;

typedef struct {
    char names[MAX_VERTICES][MAX_NAME_LEN];
    int num_vertices;
    Edge edges[MAX_EDGES];
    int num_edges;
} CityNetwork;

/* Union-Find with path compression */
void uf_init(UnionFind *uf, int n) {
    uf->size = n;
    for (int i = 0; i < n; i++) {
        uf->parent[i] = i;
        uf->rank[i] = 0;
    }
}

int uf_find(UnionFind *uf, int x) {
    if (uf->parent[x] != x) {
        uf->parent[x] = uf_find(uf, uf->parent[x]);  /* Path compression */
    }
    return uf->parent[x];
}

bool uf_union(UnionFind *uf, int x, int y) {
    int rx = uf_find(uf, x);
    int ry = uf_find(uf, y);
    
    if (rx == ry) return false;
    
    /* Union by rank */
    if (uf->rank[rx] < uf->rank[ry]) {
        uf->parent[rx] = ry;
    } else if (uf->rank[rx] > uf->rank[ry]) {
        uf->parent[ry] = rx;
    } else {
        uf->parent[ry] = rx;
        uf->rank[rx]++;
    }
    return true;
}

/* Comparison function for sorting edges */
int compare_edges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

int find_city_index(CityNetwork *net, const char *name) {
    for (int i = 0; i < net->num_vertices; i++) {
        if (strcmp(net->names[i], name) == 0) {
            return i;
        }
    }
    /* Add new city */
    strcpy(net->names[net->num_vertices], name);
    return net->num_vertices++;
}

bool read_network_file(const char *filename, CityNetwork *net) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return false;
    }
    
    net->num_vertices = 0;
    net->num_edges = 0;
    
    char city1[MAX_NAME_LEN], city2[MAX_NAME_LEN];
    int cost;
    
    while (fscanf(f, "%s %s %d", city1, city2, &cost) == 3) {
        int src = find_city_index(net, city1);
        int dest = find_city_index(net, city2);
        
        net->edges[net->num_edges].src = src;
        net->edges[net->num_edges].dest = dest;
        net->edges[net->num_edges].weight = cost;
        net->num_edges++;
    }
    
    fclose(f);
    return true;
}

void solve_mst(const char *filename) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART A: MINIMUM COST NETWORK (Kruskal's Algorithm)        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    CityNetwork net;
    if (!read_network_file(filename, &net)) return;
    
    printf("  Input Network:\n");
    printf("  ══════════════\n");
    printf("  Cities: %d\n", net.num_vertices);
    printf("  Connections: %d\n\n", net.num_edges);
    
    /* Sort edges by weight */
    qsort(net.edges, net.num_edges, sizeof(Edge), compare_edges);
    
    UnionFind uf;
    uf_init(&uf, net.num_vertices);
    
    Edge mst_edges[MAX_VERTICES];
    int mst_count = 0;
    int total_cost = 0;
    
    printf("  Kruskal's Algorithm Execution:\n");
    printf("  ═══════════════════════════════\n\n");
    
    for (int i = 0; i < net.num_edges && mst_count < net.num_vertices - 1; i++) {
        Edge e = net.edges[i];
        
        if (uf_union(&uf, e.src, e.dest)) {
            mst_edges[mst_count++] = e;
            total_cost += e.weight;
            
            printf("  [%2d] Added: %s ─── %s (cost: %d)\n",
                   mst_count, net.names[e.src], net.names[e.dest], e.weight);
        } else {
            printf("  [--] Skipped (cycle): %s ─── %s\n",
                   net.names[e.src], net.names[e.dest]);
        }
    }
    
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  MINIMUM SPANNING TREE RESULT\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    printf("  MST Edges (sorted by cost):\n");
    printf("  ┌─────────────────────────┬───────────────────────┬──────────┐\n");
    printf("  │       City 1            │       City 2          │   Cost   │\n");
    printf("  ├─────────────────────────┼───────────────────────┼──────────┤\n");
    
    for (int i = 0; i < mst_count; i++) {
        printf("  │ %-23s │ %-21s │ %8d │\n",
               net.names[mst_edges[i].src], 
               net.names[mst_edges[i].dest],
               mst_edges[i].weight);
    }
    
    printf("  └─────────────────────────┴───────────────────────┴──────────┘\n");
    printf("\n  Total Minimum Network Cost: %d\n\n", total_cost);
    
    if (mst_count < net.num_vertices - 1) {
        printf("  ⚠ Warning: Network is not fully connected!\n");
        printf("    Only %d of %d required edges found.\n\n",
               mst_count, net.num_vertices - 1);
    }
}

/* =============================================================================
 * PART B: COURSE PREREQUISITE SCHEDULER
 * =============================================================================
 * Implements both DFS-based and Kahn's (BFS-based) topological sort.
 */

typedef struct {
    char names[MAX_VERTICES][MAX_NAME_LEN];
    int adj[MAX_VERTICES][MAX_VERTICES];
    int in_degree[MAX_VERTICES];
    int num_courses;
} CourseGraph;

int find_course_index(CourseGraph *cg, const char *name) {
    for (int i = 0; i < cg->num_courses; i++) {
        if (strcmp(cg->names[i], name) == 0) {
            return i;
        }
    }
    strcpy(cg->names[cg->num_courses], name);
    return cg->num_courses++;
}

bool read_courses_file(const char *filename, CourseGraph *cg) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return false;
    }
    
    cg->num_courses = 0;
    memset(cg->adj, 0, sizeof(cg->adj));
    memset(cg->in_degree, 0, sizeof(cg->in_degree));
    
    char prereq[MAX_NAME_LEN], course[MAX_NAME_LEN];
    
    /* Format: prerequisite course (prereq must be taken before course) */
    while (fscanf(f, "%s %s", prereq, course) == 2) {
        int src = find_course_index(cg, prereq);
        int dest = find_course_index(cg, course);
        
        if (!cg->adj[src][dest]) {
            cg->adj[src][dest] = 1;
            cg->in_degree[dest]++;
        }
    }
    
    fclose(f);
    return true;
}

/* DFS-based topological sort (reverse postorder) */
bool dfs_topo_visit(CourseGraph *cg, int v, int *visited, int *result, int *idx) {
    if (visited[v] == 1) return false;  /* Cycle detected (back edge) */
    if (visited[v] == 2) return true;   /* Already processed */
    
    visited[v] = 1;  /* Mark as being processed */
    
    for (int u = 0; u < cg->num_courses; u++) {
        if (cg->adj[v][u]) {
            if (!dfs_topo_visit(cg, u, visited, result, idx)) {
                return false;
            }
        }
    }
    
    visited[v] = 2;  /* Mark as processed */
    result[--(*idx)] = v;
    return true;
}

bool dfs_topological_sort(CourseGraph *cg, int *result) {
    int visited[MAX_VERTICES] = {0};
    int idx = cg->num_courses;
    
    for (int v = 0; v < cg->num_courses; v++) {
        if (visited[v] == 0) {
            if (!dfs_topo_visit(cg, v, visited, result, &idx)) {
                return false;  /* Cycle detected */
            }
        }
    }
    return true;
}

/* Kahn's BFS-based topological sort */
bool kahn_topological_sort(CourseGraph *cg, int *result) {
    int in_deg[MAX_VERTICES];
    memcpy(in_deg, cg->in_degree, sizeof(in_deg));
    
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    
    /* Enqueue vertices with in-degree 0 */
    for (int i = 0; i < cg->num_courses; i++) {
        if (in_deg[i] == 0) {
            queue[rear++] = i;
        }
    }
    
    int count = 0;
    
    while (front < rear) {
        int v = queue[front++];
        result[count++] = v;
        
        for (int u = 0; u < cg->num_courses; u++) {
            if (cg->adj[v][u]) {
                in_deg[u]--;
                if (in_deg[u] == 0) {
                    queue[rear++] = u;
                }
            }
        }
    }
    
    return count == cg->num_courses;  /* False if cycle exists */
}

void solve_topo(const char *filename) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART B: COURSE PREREQUISITE SCHEDULER                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    CourseGraph cg;
    if (!read_courses_file(filename, &cg)) return;
    
    printf("  Input Prerequisites:\n");
    printf("  ════════════════════\n");
    printf("  Total courses: %d\n\n", cg.num_courses);
    
    printf("  Dependency Graph:\n");
    for (int i = 0; i < cg.num_courses; i++) {
        printf("  %s →", cg.names[i]);
        bool has_dep = false;
        for (int j = 0; j < cg.num_courses; j++) {
            if (cg.adj[i][j]) {
                printf(" %s", cg.names[j]);
                has_dep = true;
            }
        }
        if (!has_dep) printf(" (none)");
        printf("\n");
    }
    
    /* DFS-based topological sort */
    int dfs_result[MAX_VERTICES];
    bool dfs_success = dfs_topological_sort(&cg, dfs_result);
    
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  METHOD 1: DFS-Based Topological Sort (Reverse Postorder)\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    if (dfs_success) {
        printf("  Valid course order found:\n\n");
        for (int i = 0; i < cg.num_courses; i++) {
            printf("   Semester %d: %s\n", i + 1, cg.names[dfs_result[i]]);
        }
    } else {
        printf("  ⚠ CYCLE DETECTED! Course prerequisites are impossible to satisfy.\n");
    }
    
    /* Kahn's algorithm */
    int kahn_result[MAX_VERTICES];
    bool kahn_success = kahn_topological_sort(&cg, kahn_result);
    
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  METHOD 2: Kahn's Algorithm (BFS-Based)\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    if (kahn_success) {
        printf("  Valid course order found:\n\n");
        for (int i = 0; i < cg.num_courses; i++) {
            printf("   Semester %d: %s\n", i + 1, cg.names[kahn_result[i]]);
        }
        
        printf("\n  Both methods agree: %s\n",
               memcmp(dfs_result, kahn_result, cg.num_courses * sizeof(int)) == 0 ?
               "Yes (identical order)" : "No (different valid orders)");
    } else {
        printf("  ⚠ CYCLE DETECTED! Course prerequisites form a circular dependency.\n");
    }
    
    printf("\n");
}

/* =============================================================================
 * PART C: NETWORK RELIABILITY ANALYSIS
 * =============================================================================
 * Finds articulation points and bridges in undirected graph.
 */

typedef struct {
    char names[MAX_VERTICES][MAX_NAME_LEN];
    int adj[MAX_VERTICES][MAX_VERTICES];
    int num_nodes;
} NetworkGraph;

/* Global arrays for DFS */
static int discovery[MAX_VERTICES];
static int low[MAX_VERTICES];
static int parent_arr[MAX_VERTICES];
static bool is_articulation[MAX_VERTICES];
static int dfs_timer;

typedef struct {
    int u, v;
} BridgeEdge;

static BridgeEdge bridges[MAX_EDGES];
static int bridge_count;

void dfs_articulation(NetworkGraph *ng, int u) {
    int children = 0;
    discovery[u] = low[u] = ++dfs_timer;
    
    for (int v = 0; v < ng->num_nodes; v++) {
        if (!ng->adj[u][v]) continue;
        
        if (discovery[v] == 0) {  /* Tree edge */
            children++;
            parent_arr[v] = u;
            
            dfs_articulation(ng, v);
            
            low[u] = (low[u] < low[v]) ? low[u] : low[v];
            
            /* Articulation point conditions */
            if (parent_arr[u] == -1 && children > 1) {
                is_articulation[u] = true;
            }
            if (parent_arr[u] != -1 && low[v] >= discovery[u]) {
                is_articulation[u] = true;
            }
            
            /* Bridge condition */
            if (low[v] > discovery[u]) {
                bridges[bridge_count].u = u;
                bridges[bridge_count].v = v;
                bridge_count++;
            }
        } else if (v != parent_arr[u]) {  /* Back edge */
            low[u] = (low[u] < discovery[v]) ? low[u] : discovery[v];
        }
    }
}

int find_node_index(NetworkGraph *ng, const char *name) {
    for (int i = 0; i < ng->num_nodes; i++) {
        if (strcmp(ng->names[i], name) == 0) {
            return i;
        }
    }
    strcpy(ng->names[ng->num_nodes], name);
    return ng->num_nodes++;
}

bool read_reliability_file(const char *filename, NetworkGraph *ng) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return false;
    }
    
    ng->num_nodes = 0;
    memset(ng->adj, 0, sizeof(ng->adj));
    
    char node1[MAX_NAME_LEN], node2[MAX_NAME_LEN];
    
    while (fscanf(f, "%s %s", node1, node2) == 2) {
        int src = find_node_index(ng, node1);
        int dest = find_node_index(ng, node2);
        ng->adj[src][dest] = ng->adj[dest][src] = 1;  /* Undirected */
    }
    
    fclose(f);
    return true;
}

void solve_reliability(const char *filename) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PART C: NETWORK RELIABILITY ANALYSIS                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    NetworkGraph ng;
    if (!read_reliability_file(filename, &ng)) return;
    
    printf("  Input Network:\n");
    printf("  ══════════════\n");
    printf("  Nodes: %d\n\n", ng.num_nodes);
    
    /* Count edges */
    int edge_count = 0;
    printf("  Network Topology:\n");
    for (int i = 0; i < ng.num_nodes; i++) {
        printf("   %s ↔ ", ng.names[i]);
        bool first = true;
        for (int j = 0; j < ng.num_nodes; j++) {
            if (ng.adj[i][j]) {
                if (i < j) edge_count++;
                if (!first) printf(", ");
                printf("%s", ng.names[j]);
                first = false;
            }
        }
        printf("\n");
    }
    printf("\n  Total connections: %d\n", edge_count);
    
    /* Initialise DFS arrays */
    memset(discovery, 0, sizeof(discovery));
    memset(low, 0, sizeof(low));
    memset(is_articulation, false, sizeof(is_articulation));
    dfs_timer = 0;
    bridge_count = 0;
    
    for (int i = 0; i < ng.num_nodes; i++) {
        parent_arr[i] = -1;
    }
    
    /* Run DFS from each unvisited node */
    for (int i = 0; i < ng.num_nodes; i++) {
        if (discovery[i] == 0) {
            dfs_articulation(&ng, i);
        }
    }
    
    /* Report articulation points */
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  ARTICULATION POINTS (Critical Nodes)\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    int ap_count = 0;
    for (int i = 0; i < ng.num_nodes; i++) {
        if (is_articulation[i]) {
            ap_count++;
            printf("   • %s (removal disconnects network)\n", ng.names[i]);
        }
    }
    
    if (ap_count == 0) {
        printf("   ✓ No articulation points found.\n");
        printf("     Network is 2-vertex-connected (highly reliable).\n");
    } else {
        printf("\n   Total articulation points: %d\n", ap_count);
        printf("   ⚠ These nodes are single points of failure!\n");
    }
    
    /* Report bridges */
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  BRIDGES (Critical Connections)\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    if (bridge_count == 0) {
        printf("   ✓ No bridges found.\n");
        printf("     Network is 2-edge-connected (every connection has redundancy).\n");
    } else {
        printf("   Critical connections (removal disconnects network):\n\n");
        for (int i = 0; i < bridge_count; i++) {
            printf("   • %s ─── %s\n", 
                   ng.names[bridges[i].u], ng.names[bridges[i].v]);
        }
        printf("\n   Total bridges: %d\n", bridge_count);
        printf("   ⚠ These connections have no redundancy!\n");
    }
    
    /* Connectivity analysis */
    printf("\n  ═══════════════════════════════════════════════════════════════\n");
    printf("  NETWORK RELIABILITY SUMMARY\n");
    printf("  ═══════════════════════════════════════════════════════════════\n\n");
    
    int reliability_score = 100;
    reliability_score -= ap_count * 15;
    reliability_score -= bridge_count * 10;
    if (reliability_score < 0) reliability_score = 0;
    
    printf("   Nodes: %d\n", ng.num_nodes);
    printf("   Edges: %d\n", edge_count);
    printf("   Articulation Points: %d\n", ap_count);
    printf("   Bridges: %d\n", bridge_count);
    printf("\n");
    
    if (ap_count == 0 && bridge_count == 0) {
        printf("   Reliability: EXCELLENT (biconnected network)\n");
        printf("   • No single point of failure\n");
        printf("   • All connections have redundancy\n");
    } else if (ap_count <= 1 && bridge_count <= 1) {
        printf("   Reliability: GOOD (mostly redundant)\n");
        printf("   • Minor vulnerabilities detected\n");
        printf("   • Consider adding backup connections\n");
    } else {
        printf("   Reliability: POOR (multiple vulnerabilities)\n");
        printf("   • Critical nodes/edges should have backups\n");
        printf("   • Network redesign recommended\n");
    }
    
    printf("\n   Estimated reliability score: %d/100\n\n", reliability_score);
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("\nUsage: %s <mode> <input_file>\n\n", prog);
    printf("Modes:\n");
    printf("  mst         - Minimum Spanning Tree (network design)\n");
    printf("  topo        - Topological Sort (course scheduling)\n");
    printf("  reliability - Network reliability analysis\n");
    printf("\nExamples:\n");
    printf("  %s mst cities.txt\n", prog);
    printf("  %s topo courses.txt\n", prog);
    printf("  %s reliability network.txt\n", prog);
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                               ║\n");
    printf("║     WEEK 16: GRAPH ALGORITHMS SUITE                           ║\n");
    printf("║              Homework 1 Solution                              ║\n");
    printf("║                                                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    if (argc < 3) {
        print_usage(argv[0]);
        
        /* Demo mode with sample data */
        printf("  Running demonstration with built-in data...\n");
        
        /* Create temp files for demo */
        FILE *f;
        
        /* MST demo data */
        f = fopen("/tmp/demo_mst.txt", "w");
        fprintf(f, "Bucharest Cluj 450\n");
        fprintf(f, "Bucharest Constanta 225\n");
        fprintf(f, "Bucharest Craiova 230\n");
        fprintf(f, "Cluj Oradea 150\n");
        fprintf(f, "Cluj Timisoara 320\n");
        fprintf(f, "Timisoara Craiova 290\n");
        fprintf(f, "Constanta Galati 190\n");
        fprintf(f, "Cluj Iasi 400\n");
        fprintf(f, "Iasi Galati 220\n");
        fprintf(f, "Oradea Timisoara 210\n");
        fclose(f);
        
        /* Topo demo data */
        f = fopen("/tmp/demo_topo.txt", "w");
        fprintf(f, "Calculus1 Calculus2\n");
        fprintf(f, "Calculus2 Calculus3\n");
        fprintf(f, "Programming1 Programming2\n");
        fprintf(f, "Programming2 DataStructures\n");
        fprintf(f, "DataStructures Algorithms\n");
        fprintf(f, "Calculus2 Statistics\n");
        fprintf(f, "Statistics MachineLearning\n");
        fprintf(f, "Algorithms MachineLearning\n");
        fprintf(f, "Programming1 Databases\n");
        fprintf(f, "Databases WebDevelopment\n");
        fclose(f);
        
        /* Reliability demo data */
        f = fopen("/tmp/demo_rel.txt", "w");
        fprintf(f, "ServerA RouterA\n");
        fprintf(f, "RouterA RouterB\n");
        fprintf(f, "RouterB ServerB\n");
        fprintf(f, "ServerA ServerB\n");
        fprintf(f, "RouterA RouterC\n");
        fprintf(f, "RouterC ServerC\n");
        fclose(f);
        
        solve_mst("/tmp/demo_mst.txt");
        solve_topo("/tmp/demo_topo.txt");
        solve_reliability("/tmp/demo_rel.txt");
        
        return 0;
    }
    
    const char *mode = argv[1];
    const char *filename = argv[2];
    
    if (strcmp(mode, "mst") == 0) {
        solve_mst(filename);
    } else if (strcmp(mode, "topo") == 0) {
        solve_topo(filename);
    } else if (strcmp(mode, "reliability") == 0) {
        solve_reliability(filename);
    } else {
        fprintf(stderr, "Error: Unknown mode '%s'\n", mode);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
