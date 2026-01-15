# Extended Challenges - Week 13: Graph Algorithms

## 🚀 Advanced Challenges (Optional)

These challenges are designed for students who wish to deepen their understanding of shortest path algorithms and explore advanced applications. Each correctly solved challenge earns **+10 bonus points**.

---

## ⭐ Challenge 1: Bidirectional Dijkstra (Difficulty: Medium)

### Description

Implement bidirectional Dijkstra's algorithm that searches simultaneously from source and destination, meeting in the middle. This optimisation can significantly reduce the search space for single-pair shortest paths.

### Requirements

1. Maintain two priority queues: one for forward search (from source) and one for backward search (from destination)
2. Alternate between expanding the forward and backward frontiers
3. Terminate when the frontiers meet (a vertex is visited by both searches)
4. Correctly compute the shortest path by combining the two partial paths
5. Handle edge cases: source equals destination, no path exists

### Interface

```c
typedef struct {
    int *path;          /* Array of vertices in the path */
    int path_length;    /* Number of vertices in the path */
    int total_distance; /* Total path distance */
} PathResult;

/**
 * Bidirectional Dijkstra algorithm
 * 
 * @param graph     The graph to search
 * @param source    Starting vertex
 * @param dest      Destination vertex
 * @return          PathResult with shortest path details
 */
PathResult bidirectional_dijkstra(Graph *graph, int source, int dest);
```

### Test Case

```
Input Graph (undirected):
0 -- 1 (weight: 4)
0 -- 7 (weight: 8)
1 -- 2 (weight: 8)
1 -- 7 (weight: 11)
2 -- 3 (weight: 7)
2 -- 8 (weight: 2)
2 -- 5 (weight: 4)
3 -- 4 (weight: 9)
3 -- 5 (weight: 14)
4 -- 5 (weight: 10)
5 -- 6 (weight: 2)
6 -- 7 (weight: 1)
6 -- 8 (weight: 6)
7 -- 8 (weight: 7)

Query: shortest path from 0 to 4
Expected: 0 → 7 → 6 → 5 → 4 (distance: 21)
```

### Bonus Points: +10

---

## ⭐ Challenge 2: A* Search Algorithm (Difficulty: Medium-Hard)

### Description

Implement the A* search algorithm for finding shortest paths in graphs where heuristic information is available. A* uses a heuristic function h(n) to guide the search towards the goal, potentially examining far fewer nodes than Dijkstra's algorithm.

### Requirements

1. Implement A* with a user-provided heuristic function
2. The heuristic must be admissible (never overestimate)
3. Support different heuristic functions (Euclidean, Manhattan)
4. Track and report the number of nodes expanded
5. Compare performance with standard Dijkstra

### Interface

```c
typedef int (*HeuristicFunc)(int vertex, int goal, void *context);

typedef struct {
    int x;
    int y;
} Coordinate;

/**
 * A* search algorithm
 * 
 * @param graph       The graph to search
 * @param source      Starting vertex
 * @param dest        Destination vertex
 * @param heuristic   Heuristic function
 * @param coords      Coordinate array for heuristic calculation
 * @param dist        Output array for distances
 * @param parent      Output array for path reconstruction
 * @return            Number of nodes expanded
 */
int a_star_search(Graph *graph, int source, int dest,
                  HeuristicFunc heuristic, Coordinate *coords,
                  int *dist, int *parent);

/* Example heuristics */
int euclidean_heuristic(int vertex, int goal, void *context);
int manhattan_heuristic(int vertex, int goal, void *context);
```

### Test Scenario

Create a 10×10 grid graph with coordinates and demonstrate that A* expands fewer nodes than Dijkstra when finding a path from corner (0,0) to corner (9,9).

### Bonus Points: +10

---

## ⭐ Challenge 3: All-Pairs Shortest Paths - Floyd-Warshall (Difficulty: Medium)

### Description

Implement the Floyd-Warshall algorithm to compute shortest paths between all pairs of vertices simultaneously. This dynamic programming approach is elegant and handles negative weights (but not negative cycles).

### Requirements

1. Implement Floyd-Warshall with O(V³) complexity
2. Handle negative edge weights correctly
3. Detect negative cycles
4. Reconstruct any shortest path using the predecessor matrix
5. Memory-efficient implementation for large graphs

### Interface

```c
typedef struct {
    int **dist;         /* Distance matrix: dist[i][j] = shortest i→j */
    int **next;         /* Next vertex matrix for path reconstruction */
    int vertices;       /* Number of vertices */
    bool has_negative_cycle;
} APSPResult;

/**
 * Floyd-Warshall all-pairs shortest paths
 * 
 * @param graph     The graph to process
 * @return          APSPResult with all shortest paths
 */
APSPResult floyd_warshall(Graph *graph);

/**
 * Retrieve path between two vertices
 * 
 * @param result    Floyd-Warshall result
 * @param source    Starting vertex
 * @param dest      Destination vertex
 * @param path      Output array for path vertices
 * @return          Path length (-1 if no path)
 */
int get_path(APSPResult *result, int source, int dest, int *path);

/**
 * Free APSP result memory
 */
void free_apsp_result(APSPResult *result);
```

### Test Case

```
Input Graph:
5 vertices, edges:
0 → 1 (3), 0 → 2 (8), 0 → 4 (-4)
1 → 3 (1), 1 → 4 (7)
2 → 1 (4)
3 → 0 (2), 3 → 2 (-5)
4 → 3 (6)

Expected Distance Matrix:
     0    1    2    3    4
0    0    1   -3    2   -4
1    3    0   -4    1   -1
2    7    4    0    5    3
3    2   -1   -5    0   -2
4    8    5    1    6    0
```

### Bonus Points: +10

---

## ⭐ Challenge 4: Currency Arbitrage Detector (Difficulty: Medium-Hard)

### Description

Build a complete currency arbitrage detection system using Bellman-Ford. Given exchange rates between currencies, detect if there exists a sequence of exchanges that results in a profit (i.e. a negative cycle in the logarithmic graph).

### Requirements

1. Read exchange rates from a file in format: `FROM TO RATE`
2. Convert to logarithmic representation: `weight = -log(rate)`
3. Use Bellman-Ford to detect negative cycles
4. If arbitrage exists, output the exact sequence of trades
5. Calculate the profit percentage for one cycle

### Interface

```c
typedef struct {
    char name[4];       /* Currency code (USD, EUR, GBP, etc.) */
} Currency;

typedef struct {
    int from;
    int to;
    double rate;
} ExchangeRate;

typedef struct {
    bool exists;                /* Whether arbitrage opportunity exists */
    int *cycle;                 /* Array of currency indices in cycle */
    int cycle_length;           /* Number of currencies in cycle */
    double profit_percentage;   /* Profit from one complete cycle */
} ArbitrageResult;

/**
 * Detect currency arbitrage opportunities
 * 
 * @param currencies    Array of currency codes
 * @param num_currencies Number of currencies
 * @param rates         Array of exchange rates
 * @param num_rates     Number of exchange rates
 * @return              ArbitrageResult with details
 */
ArbitrageResult detect_arbitrage(Currency *currencies, int num_currencies,
                                  ExchangeRate *rates, int num_rates);

/**
 * Load exchange rates from file
 * 
 * @param filename      Path to exchange rates file
 * @param currencies    Output array for currencies
 * @param rates         Output array for rates
 * @return              Number of currencies loaded
 */
int load_exchange_rates(const char *filename, 
                        Currency **currencies, 
                        ExchangeRate **rates);
```

### Sample Input File (`exchange_rates.txt`)

```
USD EUR 0.91
EUR GBP 0.86
GBP USD 1.32
USD JPY 149.50
JPY EUR 0.0062
EUR CHF 0.97
CHF USD 1.12
```

### Expected Analysis

```
Checking for arbitrage opportunities...

Arbitrage detected!
Cycle: USD → EUR → GBP → USD
  1000.00 USD → 910.00 EUR (rate: 0.91)
  910.00 EUR → 782.60 GBP (rate: 0.86)
  782.60 GBP → 1033.03 USD (rate: 1.32)
  
Profit: 3.30% per cycle
```

### Bonus Points: +10

---

## ⭐ Challenge 5: Shortest Path with Constraints (Difficulty: Hard)

### Description

Implement a constrained shortest path algorithm that finds the shortest path subject to additional constraints such as:
- Maximum number of edges (hops)
- Must pass through specific vertices
- Must avoid certain vertices
- Total path weight must be within a budget

### Requirements

1. Implement modified Dijkstra/Bellman-Ford with hop constraints
2. Support "must visit" vertices (via graph transformation)
3. Support "must avoid" vertices
4. Handle combinations of constraints
5. Report if no valid path exists under constraints

### Interface

```c
typedef struct {
    int *must_visit;        /* Vertices that must be visited */
    int must_visit_count;
    int *must_avoid;        /* Vertices that must be avoided */
    int must_avoid_count;
    int max_hops;           /* Maximum number of edges (-1 for unlimited) */
    int max_weight;         /* Maximum total weight (-1 for unlimited) */
} PathConstraints;

typedef struct {
    bool valid;             /* Whether a valid path was found */
    int *path;              /* Path vertices */
    int path_length;
    int total_weight;
    int num_hops;
    char error_message[256];
} ConstrainedPathResult;

/**
 * Find shortest path with constraints
 * 
 * @param graph         The graph to search
 * @param source        Starting vertex
 * @param dest          Destination vertex
 * @param constraints   Path constraints
 * @return              ConstrainedPathResult
 */
ConstrainedPathResult shortest_path_constrained(
    Graph *graph, 
    int source, 
    int dest,
    PathConstraints *constraints
);
```

### Test Cases

```
Graph: 6 vertices
0 → 1 (2), 0 → 2 (4)
1 → 2 (1), 1 → 3 (7)
2 → 3 (3), 2 → 4 (5)
3 → 5 (1)
4 → 3 (2), 4 → 5 (3)

Test 1: Path 0→5 with max 3 hops
Expected: 0 → 2 → 3 → 5 (weight: 8, hops: 3)

Test 2: Path 0→5 must visit vertex 4
Expected: 0 → 2 → 4 → 5 (weight: 12, hops: 3)

Test 3: Path 0→5 must avoid vertex 2
Expected: 0 → 1 → 3 → 5 (weight: 10, hops: 3)

Test 4: Path 0→5 with max weight 7
Expected: No valid path (minimum is 8)
```

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "Graph Algorithm Master" badge 🏆 |

---

## 📝 Submission Guidelines

1. **File Naming**: `challenge1_bidirectional.c`, `challenge2_astar.c`, etc.
2. **Header Comment**: Include your name, student ID and brief description
3. **Compilation**: Must compile with `gcc -Wall -Wextra -std=c11 -lm`
4. **Testing**: Include test cases demonstrating correctness
5. **Documentation**: Comment complex algorithms and data structures

---

## 💡 Implementation Tips

### Challenge 1 (Bidirectional Dijkstra)
- Use two separate distance arrays and visited sets
- The meeting point may not be on the actual shortest path
- Check: `dist_forward[v] + dist_backward[v]` for all visited v

### Challenge 2 (A*)
- f(n) = g(n) + h(n) where g is actual cost, h is heuristic
- Keep the heuristic consistent for optimal results
- Use a single priority queue ordered by f(n)

### Challenge 3 (Floyd-Warshall)
- Key insight: `dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])`
- Process intermediate vertices in outer loop
- Detect negative cycles when `dist[i][i] < 0` for any i

### Challenge 4 (Arbitrage)
- Use natural logarithm: `weight = -ln(rate)`
- Finding negative cycle ≡ finding arbitrage
- Profit = `exp(-sum of cycle weights) - 1`

### Challenge 5 (Constrained Paths)
- For hop constraints: augment state to (vertex, hops_remaining)
- For must-visit: solve as TSP subproblem or use state bitmask
- Consider dynamic programming for complex constraints

---

## 🔗 Additional Resources

- Cormen et al., "Introduction to Algorithms", Chapter 24-25
- Sedgewick & Wayne, "Algorithms", Chapter 4.4
- Stanford CS161 Lecture Notes on Shortest Paths
- Competitive Programming 3 by Steven Halim, Chapter 4

---

## ⏰ Deadline

Extended challenges have a flexible deadline: **end of semester**

Submit whenever ready - earlier submissions receive priority feedback.

---

*Good luck, and may your paths always be the shortest!* 🎯
