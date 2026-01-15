# Week 14: Advanced Topics and Comprehensive Review

## 🎯 Learning Objectives

Upon successful completion of this week's laboratory, students will be able to:

1. **Remember** — Recall the fundamental characteristics, time complexities and space requirements of all major data structures and algorithms covered throughout the semester, including dynamic programming paradigms, graph traversal strategies and tree-based operations.

2. **Understand** — Explain the theoretical underpinnings of algorithm design techniques such as divide-and-conquer, greedy methods, dynamic programming and backtracking, articulating when each paradigm offers optimal solutions.

3. **Apply** — Implement hybrid solutions that combine multiple algorithmic techniques, integrating data structures such as hash tables with graph algorithms or priority queues with dynamic programming.

4. **Analyse** — Evaluate competing algorithmic approaches for complex problems, performing rigorous asymptotic analysis and identifying bottlenecks through profiling and benchmarking.

5. **Evaluate** — Critically assess trade-offs between time complexity, space complexity, code maintainability and implementation effort when selecting algorithms for real-world applications.

6. **Create** — Design and implement original solutions to novel problems by synthesising knowledge from the entire course, producing optimised, well-documented and thoroughly-tested C programs.

---

## 📜 Historical Context

The systematic study of algorithms as a formal discipline emerged from the pioneering work of mathematicians and computer scientists throughout the twentieth century. The very word "algorithm" derives from the name of the ninth-century Persian mathematician Muḥammad ibn Mūsā al-Khwārizmī, whose treatise on Hindu-Arabic numerals introduced systematic computational procedures to the Islamic world and, subsequently, to medieval Europe.

The modern era of algorithm analysis began in earnest with the work of Donald Knuth, whose monumental multi-volume opus "The Art of Computer Programming" (commencing 1968) established rigorous mathematical foundations for analysing computational procedures. Knuth introduced the now-ubiquitous O-notation to computer science, formalising asymptotic analysis and enabling precise comparisons between competing approaches.

Dynamic programming, one of the most powerful algorithmic paradigms, was developed by Richard Bellman in the 1950s whilst working at the RAND Corporation. Bellman chose the term "dynamic programming" partly for its impressive-sounding nature and partly to obscure the mathematical research from sceptical government officials. His work revolutionised optimisation problems across operations research, economics and computer science.

### Key Figure: Donald Ervin Knuth (1938–present)

Donald Knuth stands as one of the most influential computer scientists in history. Born in Milwaukee, Wisconsin, Knuth demonstrated exceptional mathematical aptitude from youth, famously finding all errors in the textbook his teachers used. He earned his Ph.D. from the California Institute of Technology in 1963 and joined Stanford University's faculty, where he remains Professor Emeritus.

Knuth's contributions extend far beyond algorithm analysis. He created the TeX typesetting system (still widely used in academic publishing), developed the METAFONT system for digital typography and introduced literate programming as a methodology for writing clear, maintainable code. His advocacy for structured programming and mathematical rigour shaped generations of computer scientists.

> *"Premature optimisation is the root of all evil."*
> — Donald E. Knuth, "Computer Programming as an Art" (1974)

---

## 📚 Theoretical Foundations

### 1. Algorithm Design Paradigms

The semester has explored four fundamental paradigms for algorithm design, each suited to different problem characteristics:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     ALGORITHM DESIGN PARADIGMS                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐        │
│   │  DIVIDE AND     │    │    DYNAMIC      │    │     GREEDY      │        │
│   │    CONQUER      │    │  PROGRAMMING    │    │    STRATEGY     │        │
│   ├─────────────────┤    ├─────────────────┤    ├─────────────────┤        │
│   │ Break into      │    │ Overlapping     │    │ Locally optimal │        │
│   │ independent     │    │ subproblems     │    │ choices lead to │        │
│   │ subproblems     │    │ with optimal    │    │ global optimum  │        │
│   │                 │    │ substructure    │    │                 │        │
│   │ Examples:       │    │                 │    │ Examples:       │        │
│   │ - Merge sort    │    │ Examples:       │    │ - Dijkstra      │        │
│   │ - Quick sort    │    │ - Fibonacci     │    │ - Prim/Kruskal  │        │
│   │ - Binary search │    │ - Knapsack      │    │ - Huffman       │        │
│   │                 │    │ - LCS           │    │                 │        │
│   └─────────────────┘    └─────────────────┘    └─────────────────┘        │
│                                                                             │
│   ┌─────────────────┐    ┌─────────────────┐                               │
│   │  BACKTRACKING   │    │   BRANCH AND    │                               │
│   │                 │    │     BOUND       │                               │
│   ├─────────────────┤    ├─────────────────┤                               │
│   │ Systematic      │    │ Backtracking    │                               │
│   │ search with     │    │ with bounds     │                               │
│   │ pruning         │    │ pruning         │                               │
│   │                 │    │                 │                               │
│   │ Examples:       │    │ Examples:       │                               │
│   │ - N-Queens      │    │ - TSP           │                               │
│   │ - Sudoku        │    │ - Integer LP    │                               │
│   │ - Graph colour  │    │ - Knapsack      │                               │
│   └─────────────────┘    └─────────────────┘                               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2. Complexity Analysis Summary

Understanding time and space complexity remains essential for algorithm selection. The following table summarises the major algorithms covered:

| Category | Algorithm | Time (Average) | Time (Worst) | Space | Stable |
|----------|-----------|----------------|--------------|-------|--------|
| **Sorting** | Quick Sort | O(n log n) | O(n²) | O(log n) | No |
| | Merge Sort | O(n log n) | O(n log n) | O(n) | Yes |
| | Heap Sort | O(n log n) | O(n log n) | O(1) | No |
| **Searching** | Binary Search | O(log n) | O(log n) | O(1) | — |
| | Hash Table | O(1) | O(n) | O(n) | — |
| | BST Search | O(log n) | O(n) | O(1) | — |
| | AVL Search | O(log n) | O(log n) | O(1) | — |
| **Graph** | BFS/DFS | O(V + E) | O(V + E) | O(V) | — |
| | Dijkstra (heap) | O((V+E) log V) | O((V+E) log V) | O(V) | — |
| | Bellman-Ford | O(VE) | O(VE) | O(V) | — |
| | Prim (heap) | O((V+E) log V) | O((V+E) log V) | O(V) | — |
| | Kruskal | O(E log E) | O(E log E) | O(V) | — |
| **Dynamic Prog.** | Fibonacci (memo) | O(n) | O(n) | O(n) | — |
| | 0/1 Knapsack | O(nW) | O(nW) | O(nW) | — |
| | LCS | O(mn) | O(mn) | O(mn) | — |
| | Edit Distance | O(mn) | O(mn) | O(mn) | — |

### 3. Memory Optimisation Techniques

Dynamic programming algorithms often consume substantial memory. Several techniques reduce space requirements:

```c
/* Space-optimised LCS: O(min(m,n)) instead of O(mn) */
int lcs_optimised(const char *X, const char *Y, int m, int n) {
    /* Ensure X is the shorter string for minimal memory */
    if (m > n) {
        const char *temp = X; X = Y; Y = temp;
        int t = m; m = n; n = t;
    }
    
    int *prev = calloc(m + 1, sizeof(int));
    int *curr = calloc(m + 1, sizeof(int));
    
    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= m; i++) {
            if (X[i-1] == Y[j-1])
                curr[i] = prev[i-1] + 1;
            else
                curr[i] = (prev[i] > curr[i-1]) ? prev[i] : curr[i-1];
        }
        int *temp = prev; prev = curr; curr = temp;
    }
    
    int result = prev[m];
    free(prev); free(curr);
    return result;
}
```

**Key Optimisation Strategies:**

- **Rolling arrays** — Maintain only necessary rows/columns of DP tables
- **Bit manipulation** — Pack boolean states into integers for subset problems
- **Sparse representations** — Use adjacency lists over matrices for sparse graphs
- **In-place algorithms** — Modify input rather than allocating new structures
- **Cache-aware design** — Organise data access patterns for CPU cache efficiency

---

## 🏭 Industrial Applications

### 1. Network Routing (Dijkstra's Algorithm)

Modern internet routing protocols such as OSPF (Open Shortest Path First) employ variants of Dijkstra's algorithm:

```c
/* Simplified OSPF cost calculation */
typedef struct {
    int destination;
    int cost;
    int next_hop;
} RoutingEntry;

void ospf_calculate_routes(Graph *topology, int router_id, 
                           RoutingEntry *table, int *table_size) {
    int *dist = malloc(topology->V * sizeof(int));
    int *prev = malloc(topology->V * sizeof(int));
    
    dijkstra(topology, router_id, dist, prev);
    
    *table_size = 0;
    for (int v = 0; v < topology->V; v++) {
        if (v != router_id && dist[v] != INT_MAX) {
            table[*table_size].destination = v;
            table[*table_size].cost = dist[v];
            table[*table_size].next_hop = trace_next_hop(prev, router_id, v);
            (*table_size)++;
        }
    }
    
    free(dist); free(prev);
}
```

### 2. Database Query Optimisation

Relational databases use dynamic programming for join ordering in query execution:

```c
/* Conceptual DP for optimal join order (System R algorithm) */
typedef struct {
    unsigned long long relation_set;  /* Bitmask of included relations */
    double cost;
    struct JoinPlan *plan;
} DPEntry;

DPEntry dp_find_optimal_join(Relation *relations, int n, 
                              Statistics *stats) {
    int total_sets = 1 << n;
    DPEntry *dp = calloc(total_sets, sizeof(DPEntry));
    
    /* Base cases: single relations */
    for (int i = 0; i < n; i++) {
        dp[1 << i].relation_set = 1 << i;
        dp[1 << i].cost = estimate_scan_cost(relations[i], stats);
    }
    
    /* DP: build larger sets from smaller subsets */
    for (int size = 2; size <= n; size++) {
        for (unsigned long long S = 0; S < total_sets; S++) {
            if (__builtin_popcountll(S) != size) continue;
            
            /* Try all partitions of S into two non-empty subsets */
            for (unsigned long long S1 = (S - 1) & S; S1 > 0; S1 = (S1 - 1) & S) {
                unsigned long long S2 = S ^ S1;
                double join_cost = dp[S1].cost + dp[S2].cost + 
                                   estimate_join_cost(S1, S2, stats);
                if (join_cost < dp[S].cost || dp[S].cost == 0) {
                    dp[S].cost = join_cost;
                    /* Store plan details... */
                }
            }
        }
    }
    
    DPEntry result = dp[total_sets - 1];
    free(dp);
    return result;
}
```

### 3. Compiler Register Allocation

Graph colouring algorithms underpin register allocation in optimising compilers:

```c
/* Register allocation via graph colouring (simplified) */
typedef struct {
    int *neighbours;
    int degree;
    int colour;  /* -1 = uncoloured, 0..K-1 = register */
    bool spilled;
} InterferenceNode;

void allocate_registers(InterferenceNode *graph, int n_vars, int K) {
    /* Build simplify worklist (degree < K) */
    int *stack = malloc(n_vars * sizeof(int));
    int sp = 0;
    
    bool *removed = calloc(n_vars, sizeof(bool));
    
    /* Iteratively simplify */
    bool progress = true;
    while (progress) {
        progress = false;
        for (int i = 0; i < n_vars; i++) {
            if (!removed[i] && effective_degree(graph, i, removed) < K) {
                stack[sp++] = i;
                removed[i] = true;
                progress = true;
            }
        }
    }
    
    /* Check for potential spills */
    for (int i = 0; i < n_vars; i++) {
        if (!removed[i]) {
            /* Must spill: choose based on heuristics */
            int victim = select_spill_candidate(graph, n_vars, removed);
            stack[sp++] = victim;
            removed[victim] = true;
            graph[victim].spilled = true;
        }
    }
    
    /* Pop and colour */
    while (sp > 0) {
        int v = stack[--sp];
        removed[v] = false;
        graph[v].colour = find_available_colour(graph, v, K);
    }
    
    free(stack); free(removed);
}
```

### 4. Version Control Systems (LCS for Diff)

Git and other version control systems use LCS variants for computing file differences:

```c
/* Generate unified diff using LCS */
typedef enum { KEEP, INSERT, DELETE } DiffOp;

typedef struct {
    DiffOp op;
    int line_num;
    char *content;
} DiffHunk;

DiffHunk *generate_diff(char **old_lines, int old_count,
                        char **new_lines, int new_count,
                        int *hunk_count) {
    /* Compute LCS with backtracking */
    int **dp = allocate_2d(old_count + 1, new_count + 1);
    
    for (int i = 1; i <= old_count; i++) {
        for (int j = 1; j <= new_count; j++) {
            if (strcmp(old_lines[i-1], new_lines[j-1]) == 0)
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = MAX(dp[i-1][j], dp[i][j-1]);
        }
    }
    
    /* Backtrack to generate diff */
    DiffHunk *hunks = malloc((old_count + new_count) * sizeof(DiffHunk));
    *hunk_count = 0;
    
    backtrack_diff(dp, old_lines, new_lines, old_count, new_count,
                   hunks, hunk_count);
    
    free_2d(dp, old_count + 1);
    return hunks;
}
```

### 5. Bioinformatics Sequence Alignment

Edit distance algorithms form the basis for DNA and protein sequence alignment:

```c
/* Smith-Waterman local alignment (simplified) */
typedef struct {
    int score;
    int start_i, start_j;
    int end_i, end_j;
    char *aligned_seq1;
    char *aligned_seq2;
} Alignment;

Alignment smith_waterman(const char *seq1, const char *seq2,
                         int match, int mismatch, int gap) {
    int m = strlen(seq1), n = strlen(seq2);
    int **H = allocate_2d(m + 1, n + 1);
    
    Alignment best = {0};
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int diag = H[i-1][j-1] + (seq1[i-1] == seq2[j-1] ? match : mismatch);
            int up = H[i-1][j] + gap;
            int left = H[i][j-1] + gap;
            
            H[i][j] = MAX(0, MAX(diag, MAX(up, left)));
            
            if (H[i][j] > best.score) {
                best.score = H[i][j];
                best.end_i = i;
                best.end_j = j;
            }
        }
    }
    
    /* Backtrack from best.end to find alignment */
    traceback_local(H, seq1, seq2, &best);
    
    free_2d(H, m + 1);
    return best;
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Algorithm Portfolio Manager

Design and implement a comprehensive algorithm benchmarking system that allows comparison of multiple algorithmic solutions for the same problem class.

**Requirements:**

1. Implement at least three sorting algorithms (Quick Sort, Merge Sort, Heap Sort)
2. Implement function pointer-based algorithm selection
3. Measure and report execution time for each algorithm
4. Generate comparative performance reports
5. Handle various input distributions (sorted, reverse sorted, random, nearly sorted)
6. Implement memory usage tracking
7. Support different data types through generic programming
8. Generate visualisation data in CSV format

### Exercise 2: Unified Graph Analyser

Build a graph analysis toolkit that integrates multiple graph algorithms into a cohesive framework.

**Requirements:**

1. Support both adjacency matrix and adjacency list representations
2. Implement BFS, DFS, Dijkstra and Bellman-Ford algorithms
3. Detect graph properties (connected, cyclic, bipartite)
4. Find shortest paths between all vertex pairs (Floyd-Warshall)
5. Compute minimum spanning trees (Prim and Kruskal)
6. Identify strongly connected components (Kosaraju's algorithm)
7. Export graph structure to DOT format for Graphviz visualisation
8. Read graphs from multiple file formats (edge list, adjacency matrix)

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make all

# Run complete example demonstration
make run

# Execute automated test suite
make test

# Check for memory leaks with Valgrind
make valgrind

# Generate performance benchmarks
make benchmark

# Clean build artefacts
make clean

# Display available targets
make help
```

---

## 📁 Directory Structure

```
week-14-advanced-review/
├── README.md                           # This documentation
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week14.html        # Main lecture slides
│   └── presentation-comparativ.html    # Language comparison slides
│
├── src/
│   ├── example1.c                      # Algorithm portfolio demonstration
│   ├── exercise1.c                     # Benchmarking exercise
│   └── exercise2.c                     # Graph analyser exercise
│
├── data/
│   ├── graph_sample.txt                # Sample graph for testing
│   └── benchmark_data.txt              # Performance test data
│
├── tests/
│   ├── test1_input.txt                 # Test input for exercise 1
│   ├── test1_expected.txt              # Expected output for exercise 1
│   ├── test2_input.txt                 # Test input for exercise 2
│   └── test2_expected.txt              # Expected output for exercise 2
│
├── teme/
│   ├── homework-requirements.md        # Homework specifications
│   └── homework-extended.md            # Bonus challenges
│
└── solution/
    ├── exercise1_sol.c                 # Exercise 1 solution
    ├── exercise2_sol.c                 # Exercise 2 solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential References

1. **Cormen, T.H., Leiserson, C.E., Rivest, R.L. & Stein, C.** (2022). *Introduction to Algorithms* (4th ed.). MIT Press. — The definitive algorithms textbook, comprehensive coverage of all topics.

2. **Sedgewick, R. & Wayne, K.** (2011). *Algorithms* (4th ed.). Addison-Wesley. — Excellent practical approach with Java implementations easily adaptable to C.

3. **Skiena, S.S.** (2020). *The Algorithm Design Manual* (3rd ed.). Springer. — Outstanding "war stories" connecting theory to practice.

### Advanced Topics

4. **Kleinberg, J. & Tardos, É.** (2006). *Algorithm Design*. Pearson. — Superb treatment of algorithm design techniques with formal proofs.

5. **Knuth, D.E.** (1997–2011). *The Art of Computer Programming*, Vols. 1–4A. Addison-Wesley. — The definitive reference for serious algorithm study.

6. **Dasgupta, S., Papadimitriou, C.H. & Vazirani, U.V.** (2006). *Algorithms*. McGraw-Hill. — Concise yet rigorous treatment with excellent complexity analysis.

### Online Resources

7. **MIT OpenCourseWare 6.006** — Introduction to Algorithms (video lectures)
   https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/

8. **Visualgo** — Algorithm Visualisations
   https://visualgo.net/

9. **Big-O Cheat Sheet** — Complexity reference
   https://www.bigocheatsheet.com/

---

## ✅ Self-Assessment Checklist

Before completing this week, verify your understanding:

- [ ] I can identify the appropriate algorithm design paradigm for a given problem
- [ ] I can perform asymptotic analysis and express complexity using O, Θ and Ω notation
- [ ] I can implement and compare multiple sorting algorithms
- [ ] I can implement dynamic programming solutions with space optimisation
- [ ] I can implement graph algorithms using both adjacency matrix and list representations
- [ ] I can analyse algorithm performance using profiling tools
- [ ] I can detect and fix memory leaks using Valgrind
- [ ] I can design test cases that exercise edge cases and boundary conditions
- [ ] I can explain the trade-offs between different algorithmic approaches
- [ ] I can write clean, well-documented C code following best practices

---

## 💼 Interview Preparation

Common technical interview questions on algorithms and data structures:

1. **"Describe the differences between BFS and DFS. When would you use each?"**
   — Focus on space complexity (BFS: O(branching factor^depth) vs DFS: O(depth)), applications (BFS for shortest path in unweighted graphs, DFS for topological sort).

2. **"How would you detect a cycle in a linked list? What is the time and space complexity?"**
   — Floyd's tortoise and hare algorithm, O(n) time, O(1) space.

3. **"Explain dynamic programming. How does it differ from simple recursion?"**
   — Memoisation/tabulation of overlapping subproblems, optimal substructure property. Reduces exponential to polynomial complexity.

4. **"Given a graph, how would you find the shortest path between two nodes?"**
   — Discuss BFS (unweighted), Dijkstra (non-negative weights), Bellman-Ford (handles negative weights), Floyd-Warshall (all pairs).

5. **"What data structure would you use to implement a priority queue? Why?"**
   — Binary heap: O(log n) insert/extract-min, O(1) find-min. Discuss heap sort, Dijkstra's algorithm applications.

---

## 🔗 Course Conclusion

This concludes the Algorithms and Programming Techniques course. Throughout fourteen weeks, we have progressed from foundational concepts in function pointers and file handling through sophisticated data structures (linked lists, trees, heaps, hash tables and graphs) to advanced algorithmic techniques (dynamic programming, graph algorithms and optimisation methods).

The skills acquired form the essential toolkit for software engineering:

- **Analytical thinking** — Breaking complex problems into manageable subproblems
- **Algorithmic literacy** — Recognising common patterns and selecting appropriate solutions
- **Implementation discipline** — Writing correct, efficient and maintainable code
- **Testing rigour** — Verifying correctness through systematic testing

Continue practising by solving problems on platforms such as LeetCode, HackerRank and Codeforces. The algorithmic foundations established here will serve throughout your computing careers.

*"Computer science is no more about computers than astronomy is about telescopes."*
— Edsger W. Dijkstra

---

**Prepared for:** ASE-CSIE Algorithms and Programming Techniques  
**Week:** 14 of 14  
**Last Updated:** January 2026
