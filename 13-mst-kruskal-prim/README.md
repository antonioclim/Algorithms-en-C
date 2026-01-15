# Week 13: Graph Algorithms — Shortest Path

## 🎯 Learning Objectives

By the end of this laboratory session, students will be able to:

1. **Remember** the fundamental definitions of weighted graphs, path costs and the single-source shortest path problem, recalling the key differences between Dijkstra's algorithm and Bellman-Ford.

2. **Understand** how priority queues optimise Dijkstra's algorithm and why relaxation is the central operation in both algorithms, explaining the role of negative-weight edges in algorithm selection.

3. **Apply** Dijkstra's algorithm to compute shortest paths in graphs with non-negative edge weights, implementing efficient priority queue operations using a binary heap.

4. **Analyse** the time and space complexity of shortest path algorithms, comparing O(V²) matrix-based versus O((V+E) log V) heap-based implementations.

5. **Evaluate** which shortest path algorithm is appropriate for a given problem, considering edge weight constraints, graph density and the presence of negative cycles.

6. **Create** complete implementations of both Dijkstra's and Bellman-Ford algorithms in C, integrating them with adjacency list representations and producing path reconstruction output.

---

## 📜 Historical Context

The quest for efficient shortest path algorithms represents one of the foundational chapters in the history of computer science and operations research. The problem emerged from practical concerns in telecommunications, transportation networks and logistics during the mid-twentieth century.

**Edsger W. Dijkstra** developed his eponymous algorithm in 1956 whilst working at the Mathematical Centre in Amsterdam, though it was not published until 1959. According to Dijkstra's own account, he conceived the algorithm in approximately twenty minutes whilst sitting at a café terrace with his fiancée, seeking an elegant demonstration of the capabilities of the newly-constructed ARMAC computer. The algorithm became a cornerstone of network routing protocols and remains the foundation of link-state routing in modern networks such as OSPF.

**Richard Bellman** formulated his dynamic programming approach to shortest paths around the same period, with contributions from Lester Ford Jr. leading to the combined "Bellman-Ford" algorithm by 1958. This algorithm's ability to handle negative edge weights—whilst also detecting negative cycles—made it invaluable for financial arbitrage detection, network flow analysis and constraint satisfaction problems.

### Key Figure: Edsger Wybe Dijkstra (1930–2002)

Dijkstra was a Dutch computer scientist whose contributions shaped the theoretical foundations of programming. Beyond his shortest path algorithm, he pioneered structured programming, introduced the semaphore concept for process synchronisation and developed the "Shunting Yard" algorithm for expression parsing. He received the Turing Award in 1972.

> *"Computer Science is no more about computers than astronomy is about telescopes."*
> — Edsger W. Dijkstra

---

## 📚 Theoretical Foundations

### 1. The Single-Source Shortest Path Problem

Given a weighted directed graph G = (V, E) with weight function w: E → ℝ and a source vertex s ∈ V, find the minimum-weight path from s to every other vertex v ∈ V.

```
                    Path Weight Definition
    ═══════════════════════════════════════════════════════

    For a path p = ⟨v₀, v₁, ..., vₖ⟩:

                    k
        w(p) = Σ w(vᵢ₋₁, vᵢ)
                   i=1

    The shortest-path weight δ(s, v) is:

        δ(s, v) = min{w(p) : s ⟿ v}  if path exists
                = ∞                    otherwise
```

**Graph Representation — Adjacency List:**

```
    Vertex 0 ──→ [1, w=4] ──→ [2, w=1] ──→ NULL
           │
           ▼
    Vertex 1 ──→ [3, w=1] ──→ NULL
           │
           ▼
    Vertex 2 ──→ [1, w=2] ──→ [3, w=5] ──→ NULL
           │
           ▼
    Vertex 3 ──→ [4, w=3] ──→ NULL
```

### 2. The Relaxation Operation

Both Dijkstra's and Bellman-Ford algorithms rely upon the fundamental **relaxation** operation. Relaxation attempts to improve the current best-known distance to a vertex by considering a newly-discovered path.

```c
/* Relaxation: the heart of shortest path algorithms */
void relax(int u, int v, int weight, int dist[], int parent[]) {
    if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
        dist[v] = dist[u] + weight;
        parent[v] = u;
    }
}
```

**Visual representation of relaxation:**

```
    Before relaxation:              After relaxation:
    ══════════════════              ═════════════════

        dist[u] = 3                     dist[u] = 3
           (u)                             (u)
            │                               │
            │ w = 2                         │ w = 2
            ▼                               ▼
           (v)                             (v)
        dist[v] = 10                    dist[v] = 5  ✓

    Condition: dist[u] + w < dist[v]
               3 + 2 = 5 < 10  →  Update!
```

### 3. Dijkstra's Algorithm

Dijkstra's algorithm employs a **greedy strategy**, always extending the shortest known path. It maintains a set S of vertices whose final shortest-path weights have been determined and repeatedly selects the vertex u ∈ V − S with the minimum distance estimate.

```
    ╔═══════════════════════════════════════════════════════════════╗
    ║  DIJKSTRA'S ALGORITHM                                         ║
    ╠═══════════════════════════════════════════════════════════════╣
    ║  1. Initialise dist[s] = 0, dist[v] = ∞ for all v ≠ s        ║
    ║  2. Create priority queue Q containing all vertices           ║
    ║  3. While Q is not empty:                                     ║
    ║     a. Extract vertex u with minimum dist[u]                  ║
    ║     b. For each neighbour v of u:                             ║
    ║        - RELAX(u, v, w(u,v))                                  ║
    ║        - Update v's position in Q if improved                 ║
    ╚═══════════════════════════════════════════════════════════════╝
```

**Complexity Analysis:**

| Implementation | Extract-Min | Decrease-Key | Total Complexity |
|----------------|-------------|--------------|------------------|
| Array          | O(V)        | O(1)         | O(V²)            |
| Binary Heap    | O(log V)    | O(log V)     | O((V+E) log V)   |
| Fibonacci Heap | O(log V)    | O(1) amort.  | O(V log V + E)   |

**Critical Constraint:** Dijkstra's algorithm requires all edge weights to be **non-negative**. With negative edges, the greedy selection may finalise a vertex prematurely.

### 4. Bellman-Ford Algorithm

The Bellman-Ford algorithm handles graphs with **negative edge weights** and can detect **negative-weight cycles**. It performs |V| − 1 iterations, relaxing all edges in each iteration.

```
    ╔═══════════════════════════════════════════════════════════════╗
    ║  BELLMAN-FORD ALGORITHM                                       ║
    ╠═══════════════════════════════════════════════════════════════╣
    ║  1. Initialise dist[s] = 0, dist[v] = ∞ for all v ≠ s        ║
    ║  2. Repeat |V| - 1 times:                                     ║
    ║     - For each edge (u, v) with weight w:                     ║
    ║       - RELAX(u, v, w)                                        ║
    ║  3. For each edge (u, v) with weight w:                       ║
    ║     - If dist[u] + w < dist[v]:                               ║
    ║       - Report NEGATIVE CYCLE detected                        ║
    ╚═══════════════════════════════════════════════════════════════╝
```

**Complexity:** O(V × E) — slower than Dijkstra but handles negative weights.

**Negative Cycle Detection:**

```
           (A)
          ↙   ↖
     w=2 ↙     ↖ w=-5
        ↙       ↖
      (B) ───→ (C)
           w=1

    Cycle weight: 2 + 1 + (-5) = -2 < 0
    → Negative cycle: paths can be made arbitrarily short
```

---

## 🏭 Industrial Applications

### 1. Network Routing Protocols (OSPF)

The Open Shortest Path First protocol uses Dijkstra's algorithm in every router to compute optimal paths across an autonomous system.

```c
/* Simplified OSPF link-state routing concept */
typedef struct {
    int router_id;
    int cost;           /* Interface cost (bandwidth-derived) */
    int next_hop;       /* Next router towards destination */
} RoutingEntry;

void ospf_compute_routes(Graph *topology, int my_router_id) {
    int *distances = dijkstra(topology, my_router_id);
    /* Populate routing table with next-hop information */
}
```

### 2. GPS Navigation Systems

Modern navigation systems compute shortest paths considering distance, time and traffic conditions.

```c
/* Multi-criteria path computation */
typedef struct {
    double distance_km;
    double time_minutes;
    double fuel_cost;
} EdgeWeight;

/* Dijkstra with composite weight function */
double compute_weight(EdgeWeight *e, int priority) {
    switch (priority) {
        case SHORTEST: return e->distance_km;
        case FASTEST:  return e->time_minutes;
        case CHEAPEST: return e->fuel_cost;
    }
    return e->distance_km;
}
```

### 3. Financial Arbitrage Detection

Bellman-Ford detects negative cycles representing arbitrage opportunities in currency exchange.

```c
/* Currency arbitrage: convert weights to log scale */
/* Negative cycle in -log(rate) graph = arbitrage opportunity */
void detect_arbitrage(double exchange_rates[N][N]) {
    double weights[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            weights[i][j] = -log(exchange_rates[i][j]);
    
    if (bellman_ford_has_negative_cycle(weights, N))
        printf("Arbitrage opportunity exists!\n");
}
```

### 4. Social Network Analysis

Finding shortest paths between users reveals connection strength and influence propagation.

```c
/* Degrees of separation in social networks */
int degrees_of_separation(Graph *social, int user_a, int user_b) {
    int *distances = bfs_shortest_path(social, user_a);
    return distances[user_b];  /* Unweighted: BFS suffices */
}
```

### 5. Game Development — Pathfinding

A* algorithm extends Dijkstra with heuristics for efficient game AI navigation.

```c
/* A* pathfinding: Dijkstra + heuristic */
typedef struct {
    int f_score;  /* f = g + h */
    int g_score;  /* Actual distance from start */
    int h_score;  /* Heuristic estimate to goal */
} AStarNode;

/* Manhattan distance heuristic for grid-based games */
int heuristic(Point current, Point goal) {
    return abs(current.x - goal.x) + abs(current.y - goal.y);
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Dijkstra's Algorithm with Path Reconstruction

Implement Dijkstra's algorithm using a priority queue (binary heap) to find shortest paths from a source vertex to all other vertices. Your implementation must support path reconstruction to display the actual route, not merely the distances.

**Requirements:**
- Implement a min-heap priority queue with decrease-key operation
- Store parent pointers for path reconstruction
- Read graph from adjacency list file format
- Handle disconnected vertices (infinite distance)
- Display both distances and reconstructed paths

**Input Format (data/graph1.txt):**
```
6 9
0 1 4
0 2 2
1 2 1
1 3 5
2 1 1
2 3 8
2 4 10
3 4 2
4 5 6
```
First line: vertices edges. Following lines: source destination weight.

### Exercise 2: Bellman-Ford with Negative Cycle Detection

Implement the Bellman-Ford algorithm capable of handling negative edge weights and detecting negative-weight cycles. Apply the algorithm to a financial arbitrage detection scenario.

**Requirements:**
- Implement full Bellman-Ford with early termination optimisation
- Detect and report negative cycles
- If negative cycle exists, identify vertices involved
- Model currency exchange as a shortest path problem
- Convert exchange rates using logarithmic transformation

**Currency Exchange Data (data/currencies.txt):**
```
4
USD EUR GBP JPY
1.00 0.92 0.79 149.50
1.09 1.00 0.86 163.04
1.27 1.16 1.00 189.22
0.0067 0.0061 0.0053 1.00
```

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make all

# Run the complete demonstration
make run

# Execute Exercise 1 with test graph
make run-ex1

# Execute Exercise 2 with currency data
make run-ex2

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Display help
make help
```

---

## 📁 Directory Structure

```
week-13-graph-algorithms/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week13.html        # Main lecture (38 slides)
│   └── presentation-comparativ.html    # Language comparison (12 slides)
│
├── src/
│   ├── example1.c                      # Complete Dijkstra + Bellman-Ford demo
│   ├── exercise1.c                     # Dijkstra implementation exercise
│   └── exercise2.c                     # Bellman-Ford + arbitrage exercise
│
├── data/
│   ├── graph1.txt                      # Sample weighted graph
│   ├── graph2.txt                      # Larger test graph
│   └── currencies.txt                  # Currency exchange rates
│
├── tests/
│   ├── test1_input.txt                 # Test input for Exercise 1
│   ├── test1_expected.txt              # Expected output for Exercise 1
│   ├── test2_input.txt                 # Test input for Exercise 2
│   └── test2_expected.txt              # Expected output for Exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Main homework assignments
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Solution for Exercise 1
    ├── exercise2_sol.c                 # Solution for Exercise 2
    ├── homework1_sol.c                 # Solution for Homework 1
    └── homework2_sol.c                 # Solution for Homework 2
```

---

## 📖 Recommended Reading

### Essential
- Cormen, T.H. et al. *Introduction to Algorithms*, 4th ed., Chapters 22–24 (Graph Algorithms, Single-Source Shortest Paths)
- Sedgewick, R. & Wayne, K. *Algorithms*, 4th ed., Chapter 4.4 (Shortest Paths)
- Dijkstra, E.W. "A Note on Two Problems in Connexion with Graphs", *Numerische Mathematik* 1 (1959): 269–271

### Advanced
- Fredman, M.L. & Tarjan, R.E. "Fibonacci Heaps and Their Uses in Improved Network Optimization Algorithms", *JACM* 34 (1987): 596–615
- Johnson, D.B. "Efficient Algorithms for Shortest Paths in Sparse Networks", *JACM* 24 (1977): 1–13
- Bellman, R. "On a Routing Problem", *Quarterly of Applied Mathematics* 16 (1958): 87–90

### Online Resources
- MIT OpenCourseWare: 6.006 Introduction to Algorithms (Shortest Paths lectures)
- Visualgo: https://visualgo.net/en/sssp (Interactive shortest path visualisations)
- GeeksforGeeks: https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/

---

## ✅ Self-Assessment Checklist

Before submitting your laboratory work, verify that you can:

- [ ] Explain why Dijkstra's algorithm fails with negative edge weights
- [ ] Implement a binary heap with decrease-key operation in O(log n)
- [ ] Trace through Dijkstra's algorithm on a small graph by hand
- [ ] Describe the relaxation operation and its role in both algorithms
- [ ] Implement Bellman-Ford and explain why |V| − 1 iterations suffice
- [ ] Detect negative cycles and explain their implications
- [ ] Reconstruct the actual path from source to destination
- [ ] Compare time complexities of array vs heap implementations
- [ ] Choose the appropriate algorithm based on graph characteristics
- [ ] Apply shortest path algorithms to real-world modelling problems

---

## 💼 Interview Preparation

**Question 1:** *"Why can't Dijkstra's algorithm handle negative edge weights?"*

The algorithm's greedy strategy assumes that once a vertex is extracted from the priority queue with minimum distance, that distance is final. With negative edges, a later-discovered path through vertices not yet processed might yield a shorter distance, violating this assumption.

**Question 2:** *"What is the time complexity of Dijkstra's algorithm, and how can it be optimised?"*

The basic O(V²) implementation uses a simple array. Using a binary heap reduces this to O((V + E) log V). For very dense graphs, Fibonacci heaps achieve O(V log V + E), though the constant factors often make binary heaps preferable in practice.

**Question 3:** *"How would you detect if arbitrage is possible in a currency exchange?"*

Model currencies as vertices and exchange rates as edge weights. Convert rates using negative logarithms: w(u,v) = −log(rate(u,v)). A negative cycle in this transformed graph indicates that multiplying rates around the cycle exceeds 1.0, representing profit.

**Question 4:** *"Explain the difference between Dijkstra's, Bellman-Ford and Floyd-Warshall algorithms."*

Dijkstra (single-source, non-negative weights, O((V+E) log V)), Bellman-Ford (single-source, handles negative weights, detects negative cycles, O(VE)) and Floyd-Warshall (all-pairs, handles negative weights, O(V³)).

**Question 5:** *"How does A\* improve upon Dijkstra's algorithm for pathfinding?"*

A* adds a heuristic function h(v) estimating the remaining distance to the goal. It prioritises vertices by f(v) = g(v) + h(v), where g(v) is the actual distance from the source. With an admissible heuristic (never overestimating), A* finds optimal paths whilst exploring fewer vertices than Dijkstra.

---

## 🔗 Next Week Preview

**Week 14: Advanced Topics and Course Review**

The final week consolidates all data structures and algorithms covered throughout the course. Topics include algorithm selection strategies, complexity analysis across all covered structures, optimisation techniques and examination preparation. We shall revisit key implementations, discuss common examination questions and explore advanced topics such as amortised analysis and space-time trade-offs.

---

*© 2025 Algorithms and Programming Techniques — ASE Bucharest*
