# Week 16: Advanced Graph Algorithms & Backtracking

## 🎯 Learning Objectives

Upon completion of this laboratory, students will be able to:

1. **Remember** the cut property and cycle property for Minimum Spanning Trees
2. **Understand** why backtracking systematically explores the solution space through implicit enumeration
3. **Apply** Kruskal's and Prim's algorithms to find minimum spanning trees
4. **Analyse** when to prefer Kruskal (sparse graphs) versus Prim (dense graphs) based on graph characteristics
5. **Evaluate** pruning strategies in backtracking to reduce search space exponentially
6. **Create** backtracking solutions for constraint satisfaction problems such as N-Queens, Sudoku, and graph colouring

---

## 📜 Historical Context

### The Golden Age of Graph Algorithms (1956-1965)

The late 1950s witnessed a remarkable convergence of mathematical insight and practical computing needs, producing algorithms that remain fundamental to computer science. Three pioneers—Joseph Kruskal, Robert Prim, and Robert Tarjan—developed techniques that transformed how we think about connectivity, optimisation, and data structures.

### Joseph Bernard Kruskal (1928-2010)

An American mathematician and statistician, Kruskal published his eponymous algorithm in 1956 while working at Bell Laboratories. His elegant approach to finding minimum spanning trees—sorting edges by weight and greedily selecting non-cycle-forming edges—demonstrated how simple invariants could yield optimal solutions. Kruskal later became renowned for multidimensional scaling in statistics, but his graph algorithm remains his most widely implemented contribution.

### Robert Clay Prim (1921-2021)

A Bell Labs researcher who lived to the remarkable age of 99, Prim independently discovered an MST algorithm in 1957. His vertex-growing approach—starting from an arbitrary vertex and repeatedly adding the minimum-weight edge connecting the tree to a new vertex—provides an alternative perspective that proves more efficient for dense graphs. The algorithm was actually first discovered by Vojtěch Jarník in 1930, leading some to call it the Prim-Jarník algorithm.

### Robert Endre Tarjan (1948–)

Perhaps the most influential algorithmist alive, Tarjan received the Turing Award in 1986 for his fundamental contributions to data structures and graph algorithms. His work on Union-Find with path compression and union by rank transformed Kruskal's algorithm from O(E log E) to nearly linear time. Tarjan also developed algorithms for strongly connected components, articulation points, and dominators that remain standard today.

> *"The algorithm design process is one of the most intellectually demanding and creative activities in computer science. A good algorithm is a thing of beauty; it reveals something about the structure of the problem."*
> — Robert Tarjan

### The Backtracking Paradigm

While specific backtracking problems have ancient roots (N-Queens was posed by chess composer Max Bezzel in 1848), the systematic study of backtracking as an algorithmic paradigm emerged in the 1960s. Donald Knuth's "Dancing Links" (Algorithm X) and the development of constraint propagation techniques in artificial intelligence transformed backtracking from brute-force enumeration into a sophisticated search strategy with practical applications in scheduling, planning, and combinatorial optimisation.

---

## 📚 Theoretical Foundations

### Part A: Minimum Spanning Trees

#### 1. Fundamental Definitions

A **spanning tree** of a connected, undirected graph G = (V, E) is a subgraph T = (V, E') that:
- Contains all vertices V
- Is a tree (connected and acyclic)
- Has exactly |V| - 1 edges

A **Minimum Spanning Tree (MST)** is a spanning tree with minimum total edge weight.

```
Example Graph                    MST (Total Weight: 16)
    
     4       8                        4
  A─────B─────C                    A─────B     C
  │    /│\    │                    │      \    │
 2│   3 │ 2   │7                  2│       2   │7
  │  /  │  \  │                    │        \  │
  D─────E─────F                    D     E─────F
     9      10                              10

Note: Multiple MSTs may exist if edge weights are not unique.
```

#### 2. Theoretical Properties

**Cut Property:** For any cut (S, V-S) of the graph, the minimum-weight edge crossing the cut belongs to some MST.

**Cycle Property:** For any cycle in the graph, the maximum-weight edge does not belong to any MST (assuming unique weights).

```
Cut Property Illustration:
           CUT
            │
    S       │      V-S
   ┌────────┼──────────┐
   │   A    │    B     │
   │   │\   │   /│     │
   │   │ \  │  / │     │
   │   │  \ │ /  │     │
   │   D────X────E     │   ← Minimum edge X crossing cut
   │        │          │     is in some MST
   └────────┼──────────┘
            │
```

#### 3. Kruskal's Algorithm

**Strategy:** Edge-centric approach using Union-Find

```
ALGORITHM Kruskal(G)
    MST ← empty set
    SORT edges by weight (ascending)
    
    FOR each edge (u, v) in sorted order
        IF Find(u) ≠ Find(v) THEN       // Different components?
            ADD (u, v) to MST
            Union(u, v)                  // Merge components
        END IF
    END FOR
    
    RETURN MST

Complexity: O(E log E) for sorting + O(E α(V)) for Union-Find
          ≈ O(E log V) since E ≤ V²
```

**Union-Find Data Structure:**

```
                 UNION BY RANK                    PATH COMPRESSION
                                                  
Initial:  [0] [1] [2] [3] [4]                     Before Find(4):
                                                       0
After Union(0,1), Union(2,3):                         /
                                                     1
           0         2                              /
          /         /                              2
         1         3                              /
                                                 3
After Union(0,2):                               /
                                               4
           0                                   
          / \                                  After Find(4) with compression:
         1   2                                      0
            /                                    / | \ \
           3                                   1  2  3  4

Union by rank keeps trees balanced.            Path compression flattens trees.
Combined: α(n) ≈ O(1) amortised per operation.
```

#### 4. Prim's Algorithm

**Strategy:** Vertex-centric approach using priority queue

```
ALGORITHM Prim(G, start)
    MST ← empty set
    visited ← {start}
    PQ ← priority queue with edges from start
    
    WHILE |visited| < |V|
        (u, v, w) ← extract minimum edge from PQ
        IF v NOT in visited THEN
            ADD (u, v, w) to MST
            ADD v to visited
            ADD all edges from v to PQ
        END IF
    END WHILE
    
    RETURN MST

Complexity: 
    With binary heap:  O(E log V)
    With Fibonacci heap: O(E + V log V)
    With adjacency matrix (no heap): O(V²)
```

#### 5. Algorithm Selection Guide

```
╔═══════════════════════════════════════════════════════════════╗
║                    MST Algorithm Selection                     ║
╠═══════════════════════════════════════════════════════════════╣
║ Graph Type          │ Recommended │ Reasoning                  ║
╠═══════════════════════════════════════════════════════════════╣
║ Sparse (E ≈ V)      │ Kruskal     │ O(E log E) ≈ O(E log V)   ║
║ Dense (E ≈ V²)      │ Prim        │ O(V²) beats O(E log E)    ║
║ Edges pre-sorted    │ Kruskal     │ Sorting already done      ║
║ Parallel processing │ Kruskal     │ Edge checking is parallel ║
║ Incremental updates │ Prim        │ Easier to add vertices    ║
╚═══════════════════════════════════════════════════════════════╝
```

---

### Part B: Advanced Graph Algorithms

#### 1. Topological Sort

A **topological ordering** of a directed acyclic graph (DAG) is a linear ordering of vertices such that for every directed edge (u, v), vertex u appears before v.

```
DAG Example:                    Valid Topological Orders:
                                
    A → B → D                   A, C, B, E, D, F
    ↓   ↓   ↓                   A, B, C, D, E, F
    C → E → F                   C, A, B, E, D, F
                                (multiple valid orderings exist)
```

**DFS-Based Algorithm (Reverse Postorder):**

```
ALGORITHM TopologicalSort_DFS(G)
    visited ← empty set
    stack ← empty stack
    
    PROCEDURE DFS(v)
        ADD v to visited
        FOR each neighbour u of v
            IF u NOT in visited THEN
                DFS(u)
            END IF
        END FOR
        PUSH v onto stack    // Add after all descendants processed
    END PROCEDURE
    
    FOR each vertex v in G
        IF v NOT in visited THEN
            DFS(v)
        END IF
    END FOR
    
    RETURN stack (pop order gives topological sort)
```

**Kahn's Algorithm (BFS-Based):**

```
ALGORITHM TopologicalSort_Kahn(G)
    in_degree[v] ← count of incoming edges for all v
    queue ← all vertices with in_degree = 0
    result ← empty list
    
    WHILE queue NOT empty
        v ← DEQUEUE
        APPEND v to result
        FOR each neighbour u of v
            in_degree[u] ← in_degree[u] - 1
            IF in_degree[u] = 0 THEN
                ENQUEUE u
            END IF
        END FOR
    END WHILE
    
    IF |result| ≠ |V| THEN
        RETURN "Graph has a cycle!"
    END IF
    
    RETURN result
```

#### 2. Strongly Connected Components (SCC)

A **strongly connected component** is a maximal set of vertices such that every vertex is reachable from every other vertex within the set.

```
Directed Graph:                  SCCs:
                                
    A → B → C                   SCC₁: {A}
    ↑   ↓ ↗ ↓                   SCC₂: {B, C, D}
    │   D ← ─┘                  SCC₃: {E, F}
    │   ↓
    └── E ↔ F
```

**Kosaraju's Algorithm (Two DFS passes):**

```
ALGORITHM Kosaraju_SCC(G)
    // Pass 1: DFS to get finish times
    stack ← empty
    visited ← empty set
    
    PROCEDURE DFS1(v)
        ADD v to visited
        FOR each neighbour u of v
            IF u NOT in visited THEN DFS1(u)
        PUSH v to stack    // Record finish time order
    END
    
    FOR each v in G
        IF v NOT in visited THEN DFS1(v)
    
    // Pass 2: DFS on transpose graph in reverse finish order
    G_T ← transpose of G (reverse all edges)
    visited ← empty set
    sccs ← empty list
    
    PROCEDURE DFS2(v, component)
        ADD v to visited
        ADD v to component
        FOR each neighbour u in G_T
            IF u NOT in visited THEN DFS2(u, component)
    END
    
    WHILE stack NOT empty
        v ← POP stack
        IF v NOT in visited THEN
            component ← empty list
            DFS2(v, component)
            ADD component to sccs
    END WHILE
    
    RETURN sccs
```

#### 3. Articulation Points and Bridges

An **articulation point** (cut vertex) is a vertex whose removal disconnects the graph.
A **bridge** (cut edge) is an edge whose removal disconnects the graph.

```
                                Articulation Points: {B, D}
    A ─── B ─── C               Bridge: (D, E)
          │                     
          D                     Removing B disconnects {A} from rest
         / \                    Removing D disconnects {E,F} from rest
        E   F                   Removing edge (D,E) disconnects E
```

**DFS-Based Detection using Low-Link Values:**

```
disc[v]  = Discovery time of vertex v
low[v]   = Lowest discovery time reachable from subtree rooted at v

Articulation Point Conditions:
1. Root with ≥ 2 children in DFS tree
2. Non-root where low[child] ≥ disc[v]

Bridge Condition:
Edge (u, v) is bridge if low[v] > disc[u]
```

#### 4. Floyd-Warshall (All-Pairs Shortest Path)

```
ALGORITHM Floyd_Warshall(G)
    // Initialise distance matrix
    dist[i][j] ← weight(i,j) if edge exists, ∞ otherwise
    dist[i][i] ← 0
    
    // DP: Consider each vertex as intermediate
    FOR k FROM 1 TO n
        FOR i FROM 1 TO n
            FOR j FROM 1 TO n
                IF dist[i][k] + dist[k][j] < dist[i][j] THEN
                    dist[i][j] ← dist[i][k] + dist[k][j]
                END IF
            END FOR
        END FOR
    END FOR
    
    // Detect negative cycles
    FOR i FROM 1 TO n
        IF dist[i][i] < 0 THEN
            RETURN "Negative cycle detected!"
        END IF
    END FOR
    
    RETURN dist

Complexity: O(V³) time, O(V²) space
```

---

### Part C: Backtracking

#### 1. Paradigm Overview

Backtracking is a systematic method for exploring the solution space by:
1. Building solutions incrementally
2. Abandoning partial solutions ("pruning") when they cannot lead to valid complete solutions
3. Returning to try alternative choices

```
State-Space Tree for 3-element permutations:
                            ∅
              ┌─────────────┼─────────────┐
              1             2             3
         ┌────┴────┐   ┌────┴────┐   ┌────┴────┐
        12        13  21        23  31        32
        │          │   │          │   │          │
       123       132  213       231  312       321
       
Backtracking prunes branches that violate constraints early.
```

#### 2. General Backtracking Template

```c
void backtrack(int level, State *state) {
    if (is_solution(level, state)) {
        process_solution(state);
        return;
    }
    
    Candidate candidates[MAX];
    int num_candidates = construct_candidates(level, state, candidates);
    
    for (int i = 0; i < num_candidates; i++) {
        make_move(level, state, candidates[i]);
        backtrack(level + 1, state);
        unmake_move(level, state, candidates[i]);  // Backtrack!
    }
}
```

#### 3. N-Queens Problem

Place N queens on an N×N chessboard such that no two queens threaten each other.

```
4-Queens Solution:              Constraint Checking:
                                
  . Q . .                       For queen at (row, col):
  . . . Q                       • No queen in same row (guaranteed by construction)
  Q . . .                       • No queen in same column: col_used[]
  . . Q .                       • No queen on same diagonal: diag1[], diag2[]
                                  - diag1: row + col (constant for / diagonal)
                                  - diag2: row - col (constant for \ diagonal)

Number of solutions: N=1:1, N=4:2, N=8:92, N=12:14200
```

#### 4. Sudoku Solver

```
Strategy:
1. Find empty cell
2. Try digits 1-9
3. Check row, column, 3×3 box constraints
4. Recursively solve rest
5. Backtrack if no valid digit found

Optimisations:
• Choose cell with fewest valid options (MRV heuristic)
• Constraint propagation: eliminate impossible values early
• Naked singles: cell with only one option
• Hidden singles: value possible in only one cell of unit
```

#### 5. Graph Colouring

```
ALGORITHM GraphColoring(G, m_colors)
    color[v] ← 0 for all v
    
    FUNCTION CanColor(v, c)
        FOR each neighbour u of v
            IF color[u] = c THEN RETURN false
        RETURN true
    END
    
    FUNCTION Solve(v)
        IF v = n THEN RETURN true  // All vertices colored
        
        FOR c FROM 1 TO m_colors
            IF CanColor(v, c) THEN
                color[v] ← c
                IF Solve(v + 1) THEN RETURN true
                color[v] ← 0      // Backtrack
            END IF
        END FOR
        
        RETURN false
    END
    
    RETURN Solve(0)
```

#### 6. Subset Sum

```
Given set S and target T, find subset with sum = T.

ALGORITHM SubsetSum(S, n, target, current_sum, index, subset)
    IF current_sum = target THEN
        PRINT subset
        RETURN
    END IF
    
    IF index = n OR current_sum > target THEN
        RETURN  // Prune!
    END IF
    
    // Include current element
    ADD S[index] to subset
    SubsetSum(S, n, target, current_sum + S[index], index + 1, subset)
    
    // Exclude current element (backtrack)
    REMOVE S[index] from subset
    SubsetSum(S, n, target, current_sum, index + 1, subset)
```

---

## 🏭 Industrial Applications

### Network Infrastructure Design

Minimum spanning trees directly apply to designing minimum-cost networks:

```c
// Telecommunications: Connect cities with minimum cable cost
typedef struct {
    char *city1, *city2;
    double cable_km;
    double cost_per_km;
} CableRoute;

// Use Kruskal to find minimum cost network
MST mst = kruskal(routes, num_routes);
printf("Total infrastructure cost: $%.2f million\n", mst.total_cost);
```

### Build System Dependencies

Topological sort powers build systems like Make, Maven, and npm:

```c
// Compile dependencies in correct order
// package.json dependencies form a DAG
TopologicalOrder order = topo_sort(dependency_graph);
for (int i = 0; i < order.size; i++) {
    compile(order.packages[i]);
}
```

### Social Network Analysis

SCCs identify communities in directed social graphs:

```c
// Find tightly connected user groups
SCC *communities = kosaraju_scc(follow_graph);
for (int i = 0; i < num_communities; i++) {
    printf("Community %d: %d members\n", i, communities[i].size);
}
```

### Constraint Satisfaction in Scheduling

Backtracking solves complex scheduling problems:

```c
// University exam scheduling (graph colouring)
// Vertices = exams, edges = shared students
// Colours = time slots
int min_slots = chromatic_number(exam_conflict_graph);
Assignment schedule = graph_coloring_backtrack(graph, min_slots);
```

---

## 💻 Laboratory Exercises

### Exercise 1: MST and Topological Sort

Implement Minimum Spanning Tree algorithms and DAG processing.

**Requirements:**
- Implement Union-Find data structure with path compression and union by rank
- Implement Kruskal's algorithm using your Union-Find
- Implement Prim's algorithm using a binary heap
- Implement topological sort using both DFS and Kahn's algorithms
- Verify that Kruskal and Prim produce MSTs with the same total weight

**File:** `src/exercise1.c`

### Exercise 2: Backtracking Problems

Implement classic backtracking algorithms.

**Requirements:**
- Implement N-Queens solver that finds all solutions
- Count all solutions for N = 8 (should be 92)
- Implement Sudoku solver with basic constraint checking
- Implement Hamiltonian path detection for a given graph
- Add counters to track number of backtracks (pruning effectiveness)

**File:** `src/exercise2.c`

---

## 🔧 Compilation and Execution

```bash
# Compile all sources
make

# Run example demonstration
make run

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Show available targets
make help

# Build solution files (instructor only)
make solutions
```

---

## 📁 Directory Structure

```
16-advanced-graphs-backtracking/
├── README.md                    # This documentation
├── Makefile                     # Build automation
│
├── slides/
│   ├── presentation-week16.html # Main lecture (~40 slides)
│   └── presentation-comparativ.html  # C vs Python comparison
│
├── src/
│   ├── example1.c               # Complete MST + SCC + Backtracking demo
│   ├── exercise1.c              # MST + Topological Sort (TODO)
│   └── exercise2.c              # Backtracking problems (TODO)
│
├── data/
│   ├── graph_mst.txt            # Sample graph for MST
│   ├── graph_dag.txt            # DAG for topological sort
│   └── sudoku_puzzles.txt       # Sudoku test cases
│
├── tests/
│   ├── test1_input.txt          # Test cases for exercise 1
│   ├── test1_expected.txt       # Expected output for exercise 1
│   ├── test2_input.txt          # Test cases for exercise 2
│   └── test2_expected.txt       # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md # Two homework assignments (50 + 50 pts)
│   └── homework-extended.md     # Five bonus challenges
│
└── solution/
    ├── exercise1_sol.c          # Solution for exercise 1
    ├── exercise2_sol.c          # Solution for exercise 2
    ├── homework1_sol.c          # Homework 1 solution
    └── homework2_sol.c          # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential

- **CLRS** Chapters 21 (Data Structures for Disjoint Sets), 22 (Elementary Graph Algorithms), 23 (MST)
- **Sedgewick & Wayne**, *Algorithms* 4th Ed., Chapter 4: Graphs
- **Skiena**, *Algorithm Design Manual*, Chapters 5-6 on Graph Traversal

### Advanced

- Tarjan, R.E. (1972). "Depth-first search and linear graph algorithms." *SIAM Journal on Computing*
- Kruskal, J.B. (1956). "On the shortest spanning subtree of a graph." *Proceedings of the AMS*
- Knuth, D. (2000). "Dancing Links." *Millenial Perspectives in Computer Science*

### Online Resources

- [VisuAlgo - Graph Algorithms](https://visualgo.net/en/mst) — Interactive MST visualisation
- [CP Algorithms](https://cp-algorithms.com/) — Competitive programming reference
- [MIT OCW 6.006](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/) — Graph algorithms lectures

---

## ✅ Self-Assessment Checklist

After completing this laboratory, verify you can:

- [ ] Explain the cut property and why it guarantees MST correctness
- [ ] Implement Union-Find with both path compression and union by rank
- [ ] Trace through Kruskal's and Prim's algorithms on a small graph
- [ ] Determine when to use Kruskal vs Prim based on graph density
- [ ] Implement topological sort using both DFS and Kahn's algorithms
- [ ] Detect cycles in a directed graph during topological sort
- [ ] Explain the two-pass structure of Kosaraju's SCC algorithm
- [ ] Write a correct N-Queens backtracking solution
- [ ] Implement constraint checking for Sudoku
- [ ] Analyse the pruning effectiveness of your backtracking solutions
- [ ] Pass all automated tests: `make test`

---

## 💼 Interview Preparation

Common interview questions on these topics:

1. **Implement Union-Find** with path compression. What is the amortised time complexity?
   *Hint: α(n) is the inverse Ackermann function, effectively O(1)*

2. **When would you use Kruskal vs Prim?**
   *Hint: Consider edge count relative to vertex count*

3. **Find all courses you can take** given prerequisites (topological sort)
   *Hint: LeetCode 207, 210 - Course Schedule I & II*

4. **Solve N-Queens** and count all solutions for N = 8
   *Hint: Use column, diagonal arrays for O(1) conflict checking*

5. **Detect if a graph is bipartite** (2-colourable)
   *Hint: BFS or DFS with alternating colours*

---

## 🔗 Course Conclusion

This laboratory concludes the **Algorithms & Data Structures using C** course. Over 16 weeks, you have progressed from fundamental concepts to sophisticated algorithmic paradigms:

**Weeks 1-7:** Programming foundations, data structures
**Weeks 8-11:** Sorting, searching, linear data structures
**Weeks 12-14:** Trees, graphs, basic graph algorithms
**Week 15:** Algorithmic paradigms (Divide & Conquer, Greedy, DP)
**Week 16:** Advanced graphs and backtracking

The techniques learned—recursion, divide and conquer, dynamic programming, greedy algorithms, graph traversal, and backtracking—form the core toolkit of any professional programmer. These patterns appear repeatedly in:

- Database query optimisation
- Compiler design
- Network protocols
- Machine learning pipelines
- Game development
- Cryptography

**Continue your journey:**
- Practice on LeetCode, Codeforces, HackerRank
- Implement algorithms from scratch without references
- Study CLRS chapters corresponding to each week
- Explore advanced topics: approximation algorithms, randomised algorithms, parallel algorithms

> *"Algorithm design is more than a branch of computer science. It is the fundamental skill of programming itself."*
> — Donald Knuth

---

*Laboratory materials prepared for ADSC Course, Academy of Economic Studies - CSIE Bucharest*
*Final Week — Course Conclusion ...the END is not here*
