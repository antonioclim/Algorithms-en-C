# Extended Challenges - Week 14

## 🚀 Advanced Challenges (Optional)

These challenges extend beyond the core curriculum and are designed for students seeking deeper mastery. Each correctly solved challenge earns **+10 bonus points** towards your final grade.

**Important:** Bonus points are awarded only if both mandatory homework assignments score ≥35/50 each.

---

## ⭐ Challenge 1: Cache-Oblivious Algorithms (Difficulty: Hard)

### Description

Implement a **cache-oblivious matrix multiplication** algorithm that achieves better performance than the naive O(n³) approach by exploiting memory hierarchy without explicit knowledge of cache sizes.

### Background

Traditional matrix multiplication accesses memory in patterns that cause frequent cache misses. Cache-oblivious algorithms use recursive divide-and-conquer strategies to achieve near-optimal cache behaviour automatically.

### Requirements

1. Implement naive matrix multiplication as baseline
2. Implement recursive block multiplication (cache-oblivious variant)
3. Compare performance for matrices of size 64×64, 256×256, 512×512, 1024×1024
4. Measure and report cache miss ratios using `perf stat` or similar tools
5. Demonstrate at least 2× speedup for large matrices

### Required Structure

```c
typedef struct {
    double* data;
    int     rows;
    int     cols;
    int     stride;  /* For submatrix views */
} Matrix;

/* Naive: O(n³) with poor cache behaviour */
void matrix_multiply_naive(const Matrix* A, const Matrix* B, Matrix* C);

/* Cache-oblivious: O(n³) with O(n³/B√M) cache misses */
void matrix_multiply_recursive(const Matrix* A, const Matrix* B, Matrix* C);

/* Recursive helper for divide-and-conquer */
static void mm_recursive_helper(const Matrix* A, const Matrix* B, Matrix* C,
                                 int row_a, int col_a,
                                 int row_b, int col_b,
                                 int row_c, int col_c,
                                 int size);
```

### Expected Output

```
Cache-Oblivious Matrix Multiplication Analysis
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Matrix Size    Naive (ms)    Recursive (ms)    Speedup
──────────────────────────────────────────────────────
64×64              0.8           0.9            0.89×
256×256           52.3          28.7            1.82×
512×512          421.5         162.3            2.60×
1024×1024       3842.1        1124.8            3.42×

Cache Analysis (1024×1024):
  Naive: ~12.4M L1 cache misses
  Recursive: ~3.2M L1 cache misses
  Improvement: 74% fewer cache misses
```

### Bonus Points: +10

---

## ⭐ Challenge 2: A* Pathfinding with Heuristics (Difficulty: Medium)

### Description

Implement the **A\* search algorithm** with multiple heuristic functions for pathfinding on a weighted grid. Compare performance against Dijkstra's algorithm.

### Requirements

1. Implement A* with priority queue (binary heap)
2. Support multiple heuristics:
   - Manhattan distance (for 4-directional movement)
   - Euclidean distance (for 8-directional movement)
   - Chebyshev distance (for uniform 8-directional cost)
3. Load maps from file with obstacles
4. Visualise the search process (optional: ASCII animation)
5. Count nodes expanded and compare with Dijkstra

### Required Structure

```c
typedef struct {
    int x, y;
} Point;

typedef struct {
    int**  grid;       /* 0 = passable, 1 = obstacle */
    int    width;
    int    height;
    Point  start;
    Point  goal;
} GridMap;

typedef double (*Heuristic)(Point a, Point b);

/* Heuristic functions */
double heuristic_manhattan(Point a, Point b);
double heuristic_euclidean(Point a, Point b);
double heuristic_chebyshev(Point a, Point b);

/* A* implementation */
typedef struct {
    Point* path;
    int    length;
    int    nodes_expanded;
    double total_cost;
} PathResult;

PathResult astar_search(const GridMap* map, Heuristic h);
PathResult dijkstra_search(const GridMap* map);
```

### Map File Format

```
20 10
S.................
..###.............
..###....###......
.........###......
.........###......
..................
....###...........
....###...........
....###.......G...
..................
```

Where `S` = start, `G` = goal, `#` = obstacle, `.` = passable

### Expected Output

```
A* Pathfinding Comparison
━━━━━━━━━━━━━━━━━━━━━━━━━

Map: maze_50x50.txt (Start: 0,0 → Goal: 49,49)

Algorithm          Nodes Expanded    Path Length    Time (ms)
──────────────────────────────────────────────────────────────
Dijkstra                  2341            89          12.4
A* (Manhattan)             487            89           2.8
A* (Euclidean)             512            89           3.1
A* (Chebyshev)             445            89           2.5

A* expands 79-81% fewer nodes than Dijkstra!
```

### Bonus Points: +10

---

## ⭐ Challenge 3: Red-Black Tree Implementation (Difficulty: Hard)

### Description

Implement a **complete Red-Black Tree** with insertion, deletion, search and in-order traversal. This self-balancing BST guarantees O(log n) operations.

### Red-Black Tree Properties

1. Every node is either red or black
2. The root is always black
3. All leaves (NIL) are black
4. Red nodes cannot have red children
5. Every path from root to leaves has the same black-height

### Requirements

1. Implement insertion with rebalancing (rotations + recolouring)
2. Implement deletion with rebalancing (most complex operation)
3. Implement search, minimum, maximum, successor, predecessor
4. Verify tree properties after each operation
5. Compare performance against unbalanced BST with sorted input

### Required Structure

```c
typedef enum { RED, BLACK } Colour;

typedef struct RBNode {
    int             key;
    Colour          colour;
    struct RBNode*  left;
    struct RBNode*  right;
    struct RBNode*  parent;
} RBNode;

typedef struct {
    RBNode* root;
    RBNode* nil;    /* Sentinel node */
    int     size;
} RBTree;

/* Core operations */
RBTree* rbtree_create(void);
void    rbtree_destroy(RBTree* tree);
void    rbtree_insert(RBTree* tree, int key);
void    rbtree_delete(RBTree* tree, int key);
RBNode* rbtree_search(RBTree* tree, int key);

/* Utility functions */
int     rbtree_black_height(const RBTree* tree);
int     rbtree_verify_properties(const RBTree* tree);
void    rbtree_print(const RBTree* tree);
```

### Expected Output

```
Red-Black Tree Verification
━━━━━━━━━━━━━━━━━━━━━━━━━━━

Inserting: 7, 3, 18, 10, 22, 8, 11, 26, 2, 6

Tree structure:
        ┌── 26(R)
    ┌── 22(B)
┌── 18(R)
│   │   ┌── 11(R)
│   └── 10(B)
│       └── 8(R)
7(B)
│   ┌── 6(R)
└── 3(B)
    └── 2(R)

Properties check:
  ✓ Root is black
  ✓ No red-red violations
  ✓ Black-height consistent: 2
  ✓ All paths verified

Performance (10000 sorted insertions):
  Unbalanced BST: 1247.3 ms (degenerates to O(n))
  Red-Black Tree:    8.2 ms (maintains O(log n))
```

### Bonus Points: +10

---

## ⭐ Challenge 4: External Merge Sort (Difficulty: Medium)

### Description

Implement **external merge sort** for datasets that exceed available RAM. This technique is used in database systems for sorting large tables.

### Requirements

1. Sort files larger than available memory (simulate with 1MB limit)
2. Create sorted runs that fit in memory
3. Perform k-way merge using min-heap
4. Minimise disk I/O operations
5. Handle files with 10M+ integers

### Required Structure

```c
typedef struct {
    FILE*   file;
    int     buffer[BUFFER_SIZE];
    int     buffer_pos;
    int     buffer_count;
    int     exhausted;
} RunReader;

typedef struct {
    int     value;
    int     run_index;
} HeapEntry;

/* External sort configuration */
typedef struct {
    size_t  memory_limit;      /* Maximum RAM to use */
    int     num_ways;          /* K-way merge factor */
    char*   temp_directory;    /* For temporary run files */
} ExtSortConfig;

/* Main functions */
int  external_sort(const char* input_file, 
                   const char* output_file,
                   const ExtSortConfig* config);

int  create_sorted_runs(const char* input_file, 
                        const ExtSortConfig* config,
                        int* num_runs);

int  merge_runs(int num_runs, 
                const char* output_file,
                const ExtSortConfig* config);
```

### Expected Output

```
External Merge Sort
━━━━━━━━━━━━━━━━━━━

Input: large_data.bin (50,000,000 integers = 200 MB)
Memory limit: 1 MB
Temp directory: /tmp/sort_runs/

Phase 1: Creating sorted runs
  Created 200 runs of ~250,000 integers each
  Time: 12.4 seconds
  Disk writes: 200 MB

Phase 2: K-way merge (k=32)
  Merge passes required: 2
  Pass 1: 200 → 7 runs
  Pass 2: 7 → 1 run (final)
  Time: 28.7 seconds
  Total disk I/O: ~600 MB

Verification: PASSED (output is sorted)
Total time: 41.1 seconds
Throughput: 1.22M integers/second
```

### Bonus Points: +10

---

## ⭐ Challenge 5: Lock-Free Data Structure (Difficulty: Expert)

### Description

Implement a **lock-free stack** using atomic compare-and-swap (CAS) operations. This advanced concurrent data structure avoids traditional mutex locks.

### Background

Lock-free algorithms guarantee system-wide progress even if individual threads are delayed. They use atomic operations like CAS instead of locks.

### Requirements

1. Implement lock-free stack with push and pop
2. Use `<stdatomic.h>` for atomic operations
3. Handle the ABA problem using tagged pointers or hazard pointers
4. Demonstrate thread-safety with multiple producer/consumer threads
5. Compare performance against mutex-protected stack

### Required Structure

```c
#include <stdatomic.h>
#include <pthread.h>

typedef struct LFNode {
    int              value;
    struct LFNode*   next;
} LFNode;

typedef struct {
    _Atomic(LFNode*) top;
    _Atomic(size_t)  operation_count;
} LockFreeStack;

/* Lock-free operations */
LockFreeStack* lf_stack_create(void);
void           lf_stack_destroy(LockFreeStack* stack);
void           lf_stack_push(LockFreeStack* stack, int value);
int            lf_stack_pop(LockFreeStack* stack, int* value);

/* Mutex-based for comparison */
typedef struct {
    LFNode*         top;
    pthread_mutex_t lock;
} MutexStack;
```

### Expected Output

```
Lock-Free Stack Performance Test
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Configuration:
  Threads: 8 (4 producers, 4 consumers)
  Operations per thread: 100,000
  Total operations: 800,000

Results:
──────────────────────────────────────────────
Implementation     Time (ms)    Ops/sec      Contention
──────────────────────────────────────────────
Mutex-based         1247        641,539      High
Lock-free            312      2,564,102      None
──────────────────────────────────────────────

Speedup: 4.0×

Verification:
  ✓ All pushed values were popped
  ✓ No lost or duplicated values
  ✓ Final stack is empty

Note: Lock-free implementation shows better scaling
with increased thread count.
```

### Bonus Points: +10

---

## 📊 Bonus Point Summary

| Challenges Completed | Total Bonus | Achievement |
|---------------------|-------------|-------------|
| 1 | +10 points | Enthusiast |
| 2 | +20 points | Dedicated |
| 3 | +30 points | Expert |
| 4 | +40 points | Master |
| All 5 | +50 points | **Algorithm Grandmaster** 🏆 |

### Submission Notes

- Submit bonus challenges as separate files: `bonus1_name.c`, etc.
- Include a README explaining how to compile and run
- Bonus points are added after both mandatory homeworks are graded
- Maximum possible score with all bonuses: **150/100**

---

## 🎓 Why Attempt These Challenges?

1. **Interview preparation:** These topics appear frequently in technical interviews at top companies
2. **Research foundation:** Cache-oblivious algorithms and lock-free structures are active research areas
3. **Systems understanding:** Deep knowledge of memory hierarchy and concurrency
4. **Portfolio building:** Impressive projects for GitHub and CV

---

*"The only way to learn a new programming language is by writing programs in it."*
— Dennis Ritchie

Good luck, and enjoy the challenge!
