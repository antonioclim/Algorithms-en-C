# Week 12: Graph Fundamentals

## 🎯 Learning Objectives

Upon completion of this laboratory, students will be able to:

1. **Remember** the formal definitions and properties of graphs, vertices, edges, degrees and paths
2. **Understand** the trade-offs between adjacency matrix and adjacency list representations in terms of space and time complexity
3. **Apply** Breadth-First Search (BFS) and Depth-First Search (DFS) algorithms to traverse arbitrary graphs
4. **Analyse** the time and space complexity of graph algorithms and select appropriate data structures for specific problem constraints
5. **Evaluate** different traversal strategies for detecting cycles, finding connected components and computing shortest paths in unweighted graphs
6. **Create** complete C implementations of graph data structures with full memory management and traversal algorithms

---

## 📜 Historical Context

Graph theory stands as one of the most consequential branches of discrete mathematics, with origins tracing to a deceptively simple puzzle. In 1736, the Swiss mathematician Leonhard Euler addressed the famous "Seven Bridges of Königsberg" problem, asking whether one could traverse each of the city's seven bridges exactly once and return to the starting point. Euler's elegant proof of impossibility—demonstrating that such a walk requires exactly zero or two vertices of odd degree—established the foundational concepts of graph theory and is widely regarded as the birth of topology.

The twentieth century witnessed an explosion of graph-theoretic research driven by practical applications. Edsger Dijkstra's 1959 algorithm for shortest paths revolutionised route planning, whilst the development of network flow algorithms by Ford and Fulkerson enabled optimal resource allocation in transportation and communication networks. The emergence of social network analysis in the 1990s and 2000s, coupled with the exponential growth of the World Wide Web, transformed graph algorithms from academic curiosities into essential tools powering modern infrastructure.

Contemporary applications span virtually every domain of computer science: compiler optimisation relies on control flow graphs, database query optimisation employs dependency graphs, machine learning utilises graph neural networks, and cybersecurity analysis examines network topology for vulnerability detection. The algorithms you will implement this week—BFS and DFS—remain the workhorses of graph computation, serving as building blocks for hundreds of more sophisticated techniques.

### Key Figure: Edsger W. Dijkstra (1930–2002)

Edsger Wybe Dijkstra was a Dutch computer scientist whose contributions fundamentally shaped the discipline. Beyond his eponymous shortest-path algorithm, Dijkstra pioneered structured programming, invented the semaphore synchronisation primitive, and contributed the shunting-yard algorithm for expression parsing. His famous "Go To Statement Considered Harmful" letter (1968) sparked a paradigm shift in programming methodology. Dijkstra received the Turing Award in 1972 "for fundamental contributions to programming as a high, intellectual challenge."

> *"Computer Science is no more about computers than astronomy is about telescopes."*
> — Edsger W. Dijkstra

---

## 📚 Theoretical Foundations

### 1. Graph Definitions and Terminology

A **graph** G = (V, E) consists of a finite set V of **vertices** (or nodes) and a set E of **edges** (or arcs) connecting pairs of vertices.

```
Undirected Graph Example:           Directed Graph Example:
        
     A -------- B                      A -------→ B
     |  \      /|                      ↑  \      ↗|
     |   \    / |                      |   ↘    / |
     |    \  /  |                      |    ↘  ↗  ↓
     |     \/   |                      |     \/   |
     |     /\   |                      |     /\   |
     |    /  \  |                      |    ↗  ↘  |
     |   /    \ |                      |   /    ↘ ↓
     |  /      \|                      |  ↗      ↘|
     D -------- C                      D ←------- C
```

**Key terminology:**
- **Degree**: Number of edges incident to a vertex (in-degree and out-degree for directed graphs)
- **Path**: Sequence of vertices where consecutive vertices share an edge
- **Cycle**: Path that begins and ends at the same vertex
- **Connected**: An undirected graph where a path exists between every pair of vertices
- **Strongly connected**: A directed graph where paths exist in both directions between every pair

### 2. Graph Representations

#### Adjacency Matrix

An adjacency matrix A is a |V| × |V| matrix where A[i][j] = 1 if edge (i,j) exists, 0 otherwise.

```
Graph:                  Adjacency Matrix:
                        
    0 --- 1                 0  1  2  3
    |   / |             0 [ 0  1  0  1 ]
    |  /  |             1 [ 1  0  1  1 ]
    | /   |             2 [ 0  1  0  1 ]
    3 --- 2             3 [ 1  1  1  0 ]
```

**Properties:**
- Space complexity: O(|V|²)
- Edge lookup: O(1)
- Iterate neighbours: O(|V|)
- Best for: Dense graphs where |E| ≈ |V|²

#### Adjacency List

An adjacency list stores, for each vertex, a linked list of its neighbours.

```
Graph:                  Adjacency List:
                        
    0 --- 1             0 → [1] → [3]
    |   / |             1 → [0] → [2] → [3]
    |  /  |             2 → [1] → [3]
    | /   |             3 → [0] → [1] → [2]
    3 --- 2
```

**Properties:**
- Space complexity: O(|V| + |E|)
- Edge lookup: O(degree)
- Iterate neighbours: O(degree)
- Best for: Sparse graphs where |E| << |V|²

### 3. Traversal Algorithms: Complexity Analysis

| Operation                    | BFS         | DFS         |
|------------------------------|-------------|-------------|
| Time complexity              | O(V + E)    | O(V + E)    |
| Space complexity             | O(V)        | O(V)        |
| Data structure               | Queue       | Stack       |
| Shortest path (unweighted)   | ✓ Yes       | ✗ No        |
| Topological sort             | ✗ No        | ✓ Yes       |
| Cycle detection              | ✓ Yes       | ✓ Yes       |
| Connected components         | ✓ Yes       | ✓ Yes       |

---

## 🔬 Algorithm Deep Dive

### Breadth-First Search (BFS)

BFS explores vertices level by level, visiting all neighbours of a vertex before moving deeper. This produces a **shortest-path tree** in unweighted graphs.

```
BFS Traversal Order (starting from 0):

Level 0:    [0]
             ↓
Level 1:    [1, 3]
             ↓
Level 2:    [2]

Visit order: 0 → 1 → 3 → 2
```

**Algorithm pseudocode:**
```
BFS(G, source):
    create queue Q
    mark source as visited
    enqueue source into Q
    
    while Q is not empty:
        v = dequeue from Q
        process(v)
        
        for each neighbour u of v:
            if u is not visited:
                mark u as visited
                enqueue u into Q
```

### Depth-First Search (DFS)

DFS explores as deeply as possible along each branch before backtracking, naturally producing a **spanning tree** with discovery and finishing times useful for many applications.

```
DFS Traversal Order (starting from 0):

    0 → 1 → 2 → 3 (backtrack) → (backtrack) → (backtrack) → done
    
         0
        /
       1
      /
     2
    /
   3

Visit order: 0 → 1 → 2 → 3
```

**Algorithm pseudocode:**
```
DFS(G, v, visited):
    mark v as visited
    process(v)
    
    for each neighbour u of v:
        if u is not visited:
            DFS(G, u, visited)
```

---

## 🏭 Industrial Applications

### 1. Social Network Analysis (Facebook, LinkedIn)

```c
// Finding degrees of separation between users
typedef struct {
    int user_id;
    int *friends;
    int friend_count;
} UserNode;

int degrees_of_separation(UserNode *network, int n, int user_a, int user_b) {
    // BFS from user_a, counting levels until user_b is reached
    int *distance = calloc(n, sizeof(int));
    int *visited = calloc(n, sizeof(int));
    // ... BFS implementation returning distance[user_b]
}
```

### 2. Web Crawlers (Google, Bing)

```c
// URL frontier using BFS for breadth-first crawling
typedef struct URLNode {
    char url[2048];
    int depth;
    struct URLNode *next;
} URLNode;

void crawl_bfs(const char *seed_url, int max_depth) {
    Queue *frontier = queue_create();
    HashSet *visited = hashset_create();
    // BFS traversal of web pages
}
```

### 3. GPS Navigation (Google Maps, Waze)

```c
// Road network as weighted graph
typedef struct {
    int intersection_id;
    double latitude, longitude;
    Edge *roads;  // Adjacency list of connected roads
} Intersection;

// Dijkstra's algorithm builds upon BFS concepts
Path *find_shortest_route(RoadNetwork *network, int start, int dest);
```

### 4. Compiler Optimisation (GCC, LLVM)

```c
// Control flow graph for basic blocks
typedef struct BasicBlock {
    Instruction *instructions;
    struct BasicBlock **successors;
    int successor_count;
    int visited;  // For DFS-based analyses
} BasicBlock;

// Dominance analysis uses DFS
void compute_dominators(BasicBlock *entry);
```

### 5. Network Routing (Cisco IOS, Linux Kernel)

```c
// OSPF routing protocol uses Dijkstra's algorithm
typedef struct RouterNode {
    uint32_t router_id;
    struct Link {
        uint32_t neighbour_id;
        uint32_t cost;
    } *links;
    int link_count;
} RouterNode;
```

---

## 💻 Laboratory Exercises

### Exercise 1: Graph Representation and BFS

**Objective:** Implement a complete graph library supporting both adjacency matrix and adjacency list representations, with BFS traversal.

**Requirements:**
1. Define `Graph` structure supporting both representations
2. Implement `graph_create()`, `graph_destroy()`, `graph_add_edge()`
3. Implement `bfs_traversal()` that prints vertices in BFS order
4. Implement `bfs_shortest_path()` returning distance array
5. Implement `bfs_connected_components()` for undirected graphs
6. Handle memory allocation failures gracefully
7. Support both directed and undirected graphs
8. Test with provided sample data

**Input format:**
```
5 6          # vertices edges
0 1          # edge list
0 4
1 2
1 3
2 3
3 4
```

### Exercise 2: DFS and Applications

**Objective:** Implement DFS traversal with applications including cycle detection, topological sorting and strongly connected components.

**Requirements:**
1. Implement iterative DFS using explicit stack
2. Implement recursive DFS with discovery/finish times
3. Implement `dfs_has_cycle()` for directed graphs
4. Implement `dfs_topological_sort()` for DAGs
5. Implement `dfs_path_exists()` between two vertices
6. Track parent pointers for path reconstruction
7. Handle disconnected graphs correctly
8. Produce correctly formatted output matching expected files

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific target
make example1
make exercise1
make exercise2

# Run the complete example
make run

# Execute automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artefacts
make clean

# Display help
make help
```

**Compiler flags used:**
- `-Wall`: Enable all standard warnings
- `-Wextra`: Enable additional warnings
- `-std=c11`: Use C11 standard
- `-g`: Include debugging symbols

---

## 📁 Directory Structure

```
week-12-graph-fundamentals/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week12.html        # Main lecture (38 slides)
│   └── presentation-comparativ.html    # Pseudocode/C/Python comparison
│
├── src/
│   ├── example1.c                      # Complete working demonstration
│   ├── exercise1.c                     # BFS implementation exercise
│   └── exercise2.c                     # DFS applications exercise
│
├── data/
│   ├── graph_small.txt                 # Small test graph (5 vertices)
│   └── graph_medium.txt                # Medium test graph (10 vertices)
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected BFS output
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected DFS output
│
├── teme/
│   ├── homework-requirements.md        # 2 homework assignments
│   └── homework-extended.md            # 5 bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # BFS exercise solution
    ├── exercise2_sol.c                 # DFS exercise solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential
- Cormen, T. H., et al. *Introduction to Algorithms* (4th ed.), Chapters 20–22: Elementary Graph Algorithms
- Sedgewick, R. & Wayne, K. *Algorithms* (4th ed.), Chapter 4: Graphs
- Skiena, S. S. *The Algorithm Design Manual* (3rd ed.), Chapter 7: Graph Traversal

### Advanced
- Tarjan, R. E. "Depth-First Search and Linear Graph Algorithms" (1972) — Original DFS applications paper
- Hopcroft, J. & Tarjan, R. "Algorithm 447: Efficient Algorithms for Graph Manipulation" (1973)
- Even, S. *Graph Algorithms* (2nd ed.) — Comprehensive theoretical treatment

### Online Resources
- Visualgo Graph Traversal: https://visualgo.net/en/dfsbfs
- CP-Algorithms Graph Theory: https://cp-algorithms.com/graph/
- MIT OpenCourseWare 6.006: https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/

---

## ✅ Self-Assessment Checklist

Before submitting, ensure you can:

- [ ] Explain the difference between adjacency matrix and adjacency list representations
- [ ] State the time complexity of BFS and DFS for both representations
- [ ] Implement BFS using a queue data structure
- [ ] Implement DFS both recursively and iteratively
- [ ] Use BFS to find shortest paths in unweighted graphs
- [ ] Use DFS to detect cycles in directed graphs
- [ ] Perform topological sorting on a DAG using DFS
- [ ] Identify connected components in undirected graphs
- [ ] Analyse memory usage of graph representations for given V and E
- [ ] Debug graph algorithms using GDB and Valgrind

---

## 💼 Interview Preparation

**Common technical interview questions on graphs:**

1. **"How would you detect a cycle in an undirected graph?"**
   - Use DFS with parent tracking; a back edge to a visited non-parent indicates a cycle

2. **"What is the difference between BFS and DFS? When would you use each?"**
   - BFS: level-order, shortest paths, closer solutions first
   - DFS: deeper exploration, topological sort, uses less memory for wide graphs

3. **"How do you find the shortest path between two nodes?"**
   - Unweighted: BFS; Weighted non-negative: Dijkstra; Negative weights: Bellman-Ford

4. **"Implement a function to clone a graph."**
   - Use BFS/DFS with a hash map to track original→clone node mappings

5. **"How would you determine if a graph is bipartite?"**
   - BFS/DFS with two-colouring; graph is bipartite iff no odd-length cycles exist

---

## 🔗 Next Week Preview

**Week 13: Advanced Graph Algorithms**

Building upon this week's foundations, we will explore:
- **Dijkstra's Algorithm**: Shortest paths in weighted graphs with non-negative edges
- **Bellman-Ford Algorithm**: Handling negative edge weights and detecting negative cycles
- **Minimum Spanning Trees**: Prim's and Kruskal's algorithms
- **Network Flow**: Introduction to max-flow/min-cut problems

Ensure mastery of BFS and DFS before proceeding, as all advanced algorithms build upon these traversal techniques.

---

*Laboratory materials prepared for ATP Course, Computer Science Department*
*Week 12 of 14 — Graph Fundamentals*
