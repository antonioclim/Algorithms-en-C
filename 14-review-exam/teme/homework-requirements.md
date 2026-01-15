# Week 14 Homework: Advanced Topics and Comprehensive Review

## 📋 General Information

- **Deadline:** End of examination period
- **Total Points:** 100 (contributes 10% towards final grade)
- **Language:** C (C11 standard)
- **Compiler:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** Single `.c` file per homework via university portal

---

## 📝 Homework 1: Algorithm Toolkit Library (50 points)

### Description

Design and implement a **comprehensive algorithm toolkit** that demonstrates mastery of the entire semester's content. Your library must provide a unified interface for sorting, searching, graph algorithms and dynamic programming—all accessible through function pointers and a command-driven interface.

### Functional Requirements

| Requirement | Points |
|-------------|--------|
| **Core Data Structures** | 15 |
| Implement a generic array container with metadata | 5 |
| Implement an adjacency list graph representation | 5 |
| Implement a hash table with chaining (minimum 97 buckets) | 5 |
| **Algorithm Suite** | 20 |
| At least 3 sorting algorithms selectable via function pointer | 6 |
| Binary search with iterative and recursive variants | 4 |
| Dijkstra's shortest path algorithm | 5 |
| One dynamic programming solution (LCS, knapsack or edit distance) | 5 |
| **Interface Design** | 15 |
| Command-line argument parsing for operation selection | 5 |
| File I/O for loading and saving data | 5 |
| Formatted output with timing information | 5 |

### Required Function Signatures

```c
/* Generic comparator type for sorting and searching */
typedef int (*Comparator)(const void*, const void*);

/* Algorithm selector type */
typedef void (*SortFunction)(void* arr, size_t n, size_t size, Comparator cmp);

/* Core structure definitions */
typedef struct {
    void*  data;
    size_t count;
    size_t capacity;
    size_t elem_size;
} DynamicArray;

typedef struct {
    int**   adj;
    int**   weights;
    int*    degree;
    int     vertices;
    int     directed;
} Graph;

/* Required functions */
DynamicArray* array_create(size_t elem_size, size_t initial_cap);
void          array_destroy(DynamicArray* arr);
int           array_append(DynamicArray* arr, const void* elem);

Graph*        graph_create(int vertices, int directed);
void          graph_destroy(Graph* g);
int           graph_add_edge(Graph* g, int u, int v, int weight);
int*          dijkstra(const Graph* g, int source);

void toolkit_sort(DynamicArray* arr, const char* algorithm, Comparator cmp);
int  toolkit_search(const DynamicArray* arr, const void* key, Comparator cmp);
```

### Example Usage

```bash
# Sort integers from file using quicksort
./homework1 --sort quicksort --input numbers.txt --output sorted.txt

# Find shortest paths in a graph
./homework1 --graph roads.txt --dijkstra 0

# Run full demonstration
./homework1 --demo
```

### Input/Output Formats

**Number file format (one integer per line):**
```
42
17
85
3
99
```

**Graph file format (first line: V E, then E edges):**
```
5 7
0 1 10
0 3 5
1 2 1
1 3 2
2 4 4
3 1 3
3 2 9
```

**Expected output for `--demo`:**
```
╔═══════════════════════════════════════════════════════════════╗
║         ALGORITHM TOOLKIT - DEMONSTRATION                      ║
╚═══════════════════════════════════════════════════════════════╝

[SORTING] Testing with 1000 random integers
  QuickSort:     0.342 ms (O(n log n) average)
  MergeSort:     0.456 ms (O(n log n) guaranteed)
  HeapSort:      0.523 ms (O(n log n) in-place)

[SEARCHING] Binary search for value 42
  Found at index: 127
  Comparisons: 10

[GRAPH] Dijkstra from vertex 0 (5 vertices, 7 edges)
  To 0: 0
  To 1: 8
  To 2: 9
  To 3: 5
  To 4: 13

[DYNAMIC PROGRAMMING] Longest Common Subsequence
  String A: "ALGORITHM"
  String B: "LOGARITHM"
  LCS length: 7 ("LGRITHM")

All tests completed successfully.
```

### File: `homework1_toolkit.c`

---

## 📝 Homework 2: Performance Analysis Framework (50 points)

### Description

Create a **rigorous benchmarking framework** that empirically analyses algorithm complexity. Your framework must generate test data, measure execution time with statistical validity, produce complexity estimates and export results for visualisation.

### Functional Requirements

| Requirement | Points |
|-------------|--------|
| **Data Generation** | 12 |
| Random array generator with configurable seed | 3 |
| Sorted, reverse-sorted and partially sorted generators | 3 |
| Graph generator (random, dense, sparse, tree) | 3 |
| Reproducibility through seed control | 3 |
| **Timing Infrastructure** | 13 |
| High-resolution timing using `clock_gettime()` | 4 |
| Multiple runs per test with outlier elimination | 4 |
| Statistical measures: mean, median, std deviation | 5 |
| **Complexity Estimation** | 15 |
| Ratio test for empirical complexity classification | 5 |
| Curve fitting for O(n), O(n log n), O(n²), O(2^n) | 5 |
| Comparison with theoretical expectations | 5 |
| **Output and Visualisation** | 10 |
| CSV export for external plotting | 3 |
| ASCII chart generation for terminal display | 4 |
| Summary report with pass/fail status | 3 |

### Required Function Signatures

```c
/* Timing result structure */
typedef struct {
    double   mean_ns;
    double   median_ns;
    double   std_dev;
    double   min_ns;
    double   max_ns;
    int      runs;
    int      outliers_removed;
} TimingResult;

/* Complexity estimation */
typedef enum {
    COMPLEXITY_CONSTANT,    /* O(1)        */
    COMPLEXITY_LOGARITHMIC, /* O(log n)    */
    COMPLEXITY_LINEAR,      /* O(n)        */
    COMPLEXITY_LINEARITHMIC,/* O(n log n)  */
    COMPLEXITY_QUADRATIC,   /* O(n²)       */
    COMPLEXITY_CUBIC,       /* O(n³)       */
    COMPLEXITY_EXPONENTIAL, /* O(2^n)      */
    COMPLEXITY_UNKNOWN
} ComplexityClass;

/* Benchmark configuration */
typedef struct {
    const char* name;
    void      (*function)(void* data, size_t n);
    size_t*     test_sizes;
    int         num_sizes;
    int         runs_per_size;
    ComplexityClass expected;
} BenchmarkConfig;

/* Required functions */
int*         generate_random(size_t n, unsigned int seed);
int*         generate_sorted(size_t n, int ascending);
int*         generate_partial(size_t n, int sorted_percent, unsigned int seed);

TimingResult benchmark_function(void (*fn)(void*, size_t), 
                                void* data, size_t n, int runs);

ComplexityClass estimate_complexity(const TimingResult* results, 
                                    const size_t* sizes, int count);

void         export_csv(const char* filename, const BenchmarkConfig* cfg,
                        const TimingResult* results);

void         print_ascii_chart(const TimingResult* results, 
                               const size_t* sizes, int count);
```

### Example Usage

```bash
# Benchmark all sorting algorithms
./homework2 --benchmark sorting --sizes 100,500,1000,5000 --runs 5

# Analyse single algorithm with detailed statistics
./homework2 --analyse quicksort --csv results.csv

# Full analysis with complexity estimation
./homework2 --full-analysis --output report.txt
```

### Expected Output for `--full-analysis`

```
╔═══════════════════════════════════════════════════════════════╗
║         ALGORITHM COMPLEXITY ANALYSIS REPORT                   ║
╚═══════════════════════════════════════════════════════════════╝

Generated: 2026-01-15 14:30:00
Test Parameters: sizes=[100,500,1000,5000,10000], runs=5

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

SORTING ALGORITHMS
──────────────────────────────────────────────────────────────────
Algorithm        Expected     Measured     Status    Ratio (n→2n)
──────────────────────────────────────────────────────────────────
SelectionSort    O(n²)        O(n²)        ✓ PASS    3.98 ≈ 4
QuickSort        O(n log n)   O(n log n)   ✓ PASS    2.12 ≈ 2
MergeSort        O(n log n)   O(n log n)   ✓ PASS    2.08 ≈ 2
HeapSort         O(n log n)   O(n log n)   ✓ PASS    2.15 ≈ 2

ASCII Performance Chart (log scale):
n=100   |████                              |   0.05 ms
n=500   |████████████                      |   0.28 ms
n=1000  |████████████████████              |   0.64 ms
n=5000  |████████████████████████████████  |   3.89 ms

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

SEARCH ALGORITHMS
──────────────────────────────────────────────────────────────────
Algorithm        Expected     Measured     Status    Ratio (n→2n)
──────────────────────────────────────────────────────────────────
LinearSearch     O(n)         O(n)         ✓ PASS    2.01 ≈ 2
BinarySearch     O(log n)     O(log n)     ✓ PASS    1.03 ≈ 1

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

SUMMARY
──────────────────────────────────────────────────────────────────
Total algorithms tested: 6
Passed: 6
Failed: 0
Coverage: 100%

Results exported to: benchmark_results.csv
```

### File: `homework2_benchmark.c`

---

## 📊 Evaluation Criteria

| Criterion | Homework 1 | Homework 2 |
|-----------|------------|------------|
| Functional correctness | 20 | 20 |
| Algorithm implementation quality | 12 | 10 |
| Interface design and usability | 8 | 10 |
| Code organisation and modularity | 5 | 5 |
| Edge case and error handling | 5 | 5 |
| **Total** | **50** | **50** |

### Quality Standards

All code must:
- Compile without warnings using `gcc -Wall -Wextra -std=c11`
- Pass Valgrind memory check with zero leaks
- Handle invalid input gracefully (return error codes, not crash)
- Include comprehensive comments explaining algorithms
- Use consistent naming conventions (snake_case for functions)

### Penalties

| Violation | Penalty |
|-----------|---------|
| Compiler warnings | -10 points |
| Memory leaks (Valgrind errors) | -20 points |
| Crashes on valid input | -30 points |
| Missing required functions | -5 per function |
| Poor code formatting | -5 points |
| Plagiarism | -100% + disciplinary action |

---

## 📤 Submission Instructions

1. **File naming:** `homework1_[surname].c` and `homework2_[surname].c`
2. **Header comment:** Include your name, student ID and date
3. **Single file:** All code in one `.c` file (no separate headers)
4. **Test before submission:**
   ```bash
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_name.c -lm
   valgrind --leak-check=full ./homework1 --demo
   ```
5. **Upload:** University e-learning portal before deadline

---

## 💡 Implementation Tips

1. **Start with data structures:** Build `DynamicArray` and `Graph` first—everything else depends on them

2. **Use function pointers wisely:** Store algorithm implementations in dispatch tables for clean switching

3. **Test incrementally:** Verify each algorithm works before moving to the next

4. **Mind the clock:** Use `CLOCK_MONOTONIC` for timing, not `CLOCK_REALTIME`

5. **Handle edge cases:** Empty arrays, single-element arrays, disconnected graphs

6. **Document complexity:** Comment each function with its time and space complexity

7. **Statistical validity:** Remove outliers using IQR method (values outside Q1-1.5×IQR to Q3+1.5×IQR)

---

## 📚 Reference Materials

- **Cormen et al.** *Introduction to Algorithms* - Chapter 2 (Analysis), Chapter 22-26 (Graphs)
- **Sedgewick & Wayne** *Algorithms* - Performance analysis methodology
- GNU C Library documentation for `clock_gettime()`
- Course slides: Weeks 1-13 for algorithm implementations

---

## ❓ Questions?

Contact the teaching assistant during laboratory hours or via the course forum.

*Good luck with your final assignments!*
