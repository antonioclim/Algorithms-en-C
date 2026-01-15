# Week 15: Algorithmic Paradigms

## 🎯 Learning Objectives

By the end of this laboratory session, students will be able to:

1. **Remember** the Master Theorem formula and its three cases for solving recurrence relations
2. **Understand** the fundamental differences between divide-and-conquer, greedy, and dynamic programming paradigms
3. **Apply** MergeSort and QuickSort algorithms to sort arrays of arbitrary data types
4. **Analyse** recurrence relations to determine time complexity of recursive algorithms
5. **Evaluate** when greedy algorithms yield optimal solutions versus when dynamic programming is required
6. **Create** efficient solutions by selecting the appropriate algorithmic paradigm for specific problem characteristics

---

## 📜 Historical Context

### The Genesis of Algorithmic Thinking (1940s-1960s)

The mid-twentieth century witnessed a profound transformation in computational science as pioneers established the theoretical foundations upon which modern algorithm design rests. This period marked the transition from ad-hoc problem solving to systematic methodological frameworks.

### Key Figure: John von Neumann (1903-1957)

The Hungarian-American polymath John von Neumann invented MergeSort in 1945 whilst working on the EDVAC computer project. His insight was revolutionary: rather than sorting an array directly, one could recursively divide it into halves, sort each half, and merge the results. This "divide and conquer" approach demonstrated that complex problems could be solved by decomposing them into simpler subproblems.

Von Neumann's contributions extended far beyond sorting. He formalised the stored-program concept, developed game theory, and established the mathematical foundations of quantum mechanics. His merge sort algorithm remains optimal for comparison-based sorting, achieving O(n log n) time complexity with guaranteed performance regardless of input distribution.

### Key Figure: C.A.R. Hoare (1934-present)

Sir Charles Antony Richard Hoare developed QuickSort in 1959 whilst working as a visiting student at Moscow State University. The algorithm arose from a practical need: Hoare was developing a machine translation system and needed to sort words efficiently. His partition-based approach achieved average-case O(n log n) performance with remarkable practical speed due to excellent cache locality.

Hoare later made fundamental contributions to programming language theory, including the development of Communicating Sequential Processes (CSP) and the axiomatic basis for computer programming (Hoare Logic). He received the Turing Award in 1980.

> *"There are two ways of constructing a software design: One way is to make it so simple that there are obviously no deficiencies, and the other way is to make it so complicated that there are no obvious deficiencies."*
> — C.A.R. Hoare, 1980 Turing Award Lecture

### Key Figure: Richard Bellman (1920-1984)

Richard Ernest Bellman coined the term "dynamic programming" in the 1950s whilst working at the RAND Corporation. The name was deliberately chosen to be impressive yet vague—Bellman needed to obscure the mathematical nature of his work from his superiors who were hostile to mathematical research.

Bellman's principle of optimality states that an optimal policy has the property that whatever the initial state and initial decision are, the remaining decisions must constitute an optimal policy with regard to the state resulting from the first decision. This principle underpins countless algorithms in operations research, control theory, bioinformatics, and computer science.

---

## 📚 Theoretical Foundations

### Part A: Divide and Conquer

#### 1. Paradigm Definition

The divide-and-conquer strategy solves problems through three distinct phases:

```
┌─────────────────────────────────────────────────────────────┐
│                    DIVIDE AND CONQUER                       │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│    ┌──────────┐    ┌──────────┐    ┌──────────┐            │
│    │  DIVIDE  │ → │ CONQUER  │ → │  COMBINE  │            │
│    └──────────┘    └──────────┘    └──────────┘            │
│         │               │               │                   │
│         ▼               ▼               ▼                   │
│    Break problem   Solve smaller   Merge partial           │
│    into smaller    subproblems     solutions into          │
│    subproblems     recursively     final answer            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**When to use Divide and Conquer:**
- Problem can be broken into independent subproblems
- Subproblems are smaller instances of the original
- Solutions to subproblems can be combined efficiently

#### 2. Recurrence Relations

Divide-and-conquer algorithms naturally give rise to recurrence relations of the form:

```
T(n) = aT(n/b) + f(n)
```

Where:
- `a` = number of subproblems generated
- `n/b` = size of each subproblem
- `f(n)` = cost of dividing and combining

**Common Examples:**

| Algorithm | Recurrence | Solution |
|-----------|------------|----------|
| Binary Search | T(n) = T(n/2) + O(1) | O(log n) |
| MergeSort | T(n) = 2T(n/2) + O(n) | O(n log n) |
| Karatsuba Multiplication | T(n) = 3T(n/2) + O(n) | O(n^1.585) |
| Strassen's Matrix Multiplication | T(n) = 7T(n/2) + O(n²) | O(n^2.807) |

#### 3. The Master Theorem

The Master Theorem provides a direct solution for recurrences of the form T(n) = aT(n/b) + f(n):

```
Let c_crit = log_b(a)

Case 1: If f(n) = O(n^(c_crit - ε)) for some ε > 0
        Then T(n) = Θ(n^c_crit)
        
Case 2: If f(n) = Θ(n^c_crit · log^k(n)) for some k ≥ 0
        Then T(n) = Θ(n^c_crit · log^(k+1)(n))
        
Case 3: If f(n) = Ω(n^(c_crit + ε)) for some ε > 0
        AND a·f(n/b) ≤ c·f(n) for some c < 1 (regularity)
        Then T(n) = Θ(f(n))
```

**Intuition:**
- Case 1: Leaf-heavy — work at leaves dominates
- Case 2: Balanced — work spread evenly across all levels
- Case 3: Root-heavy — work at root dominates

#### 4. MergeSort Implementation

MergeSort exemplifies divide-and-conquer with elegant simplicity:

```c
void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;  // Avoid overflow
        
        merge_sort(arr, left, mid);      // Sort left half
        merge_sort(arr, mid + 1, right); // Sort right half
        merge(arr, left, mid, right);    // Combine
    }
}
```

**Merge Operation Visualisation:**

```
     Left Array          Right Array
    ┌───┬───┬───┐      ┌───┬───┬───┐
    │ 2 │ 5 │ 8 │      │ 1 │ 4 │ 9 │
    └───┴───┴───┘      └───┴───┴───┘
         i                  j
                    ↓
         ┌───┬───┬───┬───┬───┬───┐
         │ 1 │ 2 │ 4 │ 5 │ 8 │ 9 │  Merged
         └───┴───┴───┴───┴───┴───┘
```

**Properties:**
| Property | Value |
|----------|-------|
| Time (all cases) | O(n log n) |
| Space | O(n) auxiliary |
| Stable | Yes |
| In-place | No |
| Cache-friendly | Moderately |

#### 5. QuickSort Implementation

QuickSort achieves practical superiority through in-place partitioning:

```c
void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pivot_idx = partition(arr, low, high);
        
        quick_sort(arr, low, pivot_idx - 1);
        quick_sort(arr, pivot_idx + 1, high);
    }
}
```

**Pivot Selection Strategies:**

| Strategy | Description | Worst Case |
|----------|-------------|------------|
| First/Last element | Simple but vulnerable | Sorted input |
| Median-of-three | Choose median of first, middle, last | Crafted input |
| Random pivot | Randomised selection | Probabilistically rare |

**Partitioning Schemes:**

*Lomuto Partitioning* (simpler, more swaps):
```
Choose rightmost as pivot
Maintain invariant: arr[low..i] ≤ pivot < arr[i+1..j-1]
```

*Hoare Partitioning* (fewer swaps, bidirectional):
```
Choose first as pivot
Two pointers move towards centre, swap on violation
```

**Properties:**
| Property | Average | Worst |
|----------|---------|-------|
| Time | O(n log n) | O(n²) |
| Space | O(log n) | O(n) |
| Stable | No | No |
| In-place | Yes | Yes |
| Cache-friendly | Excellent | Excellent |

---

### Part B: Greedy Algorithms

#### 1. Greedy Choice Property

A greedy algorithm builds a solution incrementally, always making the locally optimal choice at each step. This approach succeeds when:

```
Local Optimum → Global Optimum (under specific conditions)
```

**The Greedy Choice Property** states that a globally optimal solution can be arrived at by making locally optimal choices. Not all problems exhibit this property!

#### 2. Optimal Substructure

A problem exhibits **optimal substructure** if an optimal solution to the problem contains optimal solutions to its subproblems. This property is necessary (but not sufficient) for greedy algorithms.

#### 3. Activity Selection Problem

**Problem:** Given n activities with start times s_i and finish times f_i, select the maximum number of non-overlapping activities.

```
Activities (sorted by finish time):
┌─────────────────────────────────────────────┐
│ A1: ████                                    │  [1, 4)
│ A2:    ████                                 │  [3, 5)
│ A3:         █████                           │  [5, 7)
│ A4:             █████                       │  [6, 8)
│ A5:                   ██████                │  [8, 11)
│ A6:                       ████████          │  [8, 12)
│ A7:                              ████       │  [12, 14)
└─────────────────────────────────────────────┘
          Time →

Greedy solution: A1, A3, A5, A7 (4 activities)
```

**Greedy Strategy:** Always select the activity with the earliest finish time that does not conflict with already selected activities.

**Correctness Sketch:** If there exists an optimal solution not containing the earliest-finishing activity, we can exchange the first activity in that solution with the earliest-finishing one without reducing the count.

#### 4. Huffman Coding

**Problem:** Given character frequencies, construct a prefix-free binary code minimising the expected code length.

**Prefix Code Property:** No codeword is a prefix of another. This ensures unambiguous decoding.

**Algorithm:**
1. Create leaf nodes for each character with frequency as priority
2. While more than one node remains:
   - Extract two nodes with minimum frequency
   - Create parent with combined frequency
   - Make extracted nodes children

**Example:**

```
Characters: A(45) B(13) C(12) D(16) E(9) F(5)

                    ┌─────────┐
                    │   100   │
                    └────┬────┘
                   0/    \1
              ┌────┴─┐  ┌─┴────┐
              │  A   │  │  55  │
              │  45  │  └──┬───┘
              └──────┘   0/ \1
                    ┌────┴─┐ ┌─┴────┐
                    │  25  │ │  30  │
                    └──┬───┘ └──┬───┘
                     0/ \1    0/ \1
                    C  D     B  ┌─┴─┐
                   12  16   13 │14 │
                               └─┬─┘
                               0/ \1
                               E   F
                               9   5

Codes: A=0, C=100, D=101, B=110, E=1110, F=1111
```

#### 5. Fractional Knapsack

**Problem:** Given items with weights w_i and values v_i, and capacity W, maximise total value that fits in the knapsack. Items may be taken fractionally.

**Greedy Strategy:** Sort items by value-to-weight ratio (v_i/w_i) in decreasing order. Take items greedily until capacity is exhausted.

```c
// Sort by v[i]/w[i] descending, then:
double total_value = 0.0;
for (int i = 0; i < n && capacity > 0; i++) {
    double take = (w[i] <= capacity) ? w[i] : capacity;
    total_value += take * (v[i] / w[i]);
    capacity -= take;
}
```

**Note:** This greedy approach does NOT work for 0/1 Knapsack where items must be taken entirely or not at all.

---

### Part C: Dynamic Programming Introduction

#### 1. Key Properties

Dynamic Programming (DP) is applicable when a problem exhibits:

1. **Overlapping Subproblems:** The same subproblems are solved multiple times
2. **Optimal Substructure:** An optimal solution contains optimal solutions to subproblems

```
┌──────────────────────────────────────────────────────┐
│           DYNAMIC PROGRAMMING INDICATORS             │
├──────────────────────────────────────────────────────┤
│  ✓ Can the problem be broken into subproblems?       │
│  ✓ Are subproblems reused across different paths?    │
│  ✓ Does optimal solution depend on optimal subs?     │
│  ✓ Is there a clear recurrence relation?             │
└──────────────────────────────────────────────────────┘
```

#### 2. Implementation Approaches

**Top-Down (Memoisation):**
- Natural recursive formulation
- Store computed results in lookup table
- Compute only needed subproblems
- Easier to implement from recurrence

**Bottom-Up (Tabulation):**
- Iterative computation
- Fill table in topological order
- Compute all subproblems
- Often more space-efficient

#### 3. Fibonacci: Three Implementations

```
Fibonacci Sequence: 0, 1, 1, 2, 3, 5, 8, 13, 21, ...
Recurrence: F(n) = F(n-1) + F(n-2), F(0)=0, F(1)=1
```

**Naive Recursive: O(2^n) time**

```c
int fib_naive(int n) {
    if (n <= 1) return n;
    return fib_naive(n - 1) + fib_naive(n - 2);
}
```

Recursion tree shows exponential redundancy:
```
                    fib(5)
                   /      \
              fib(4)      fib(3)
             /    \       /    \
         fib(3)  fib(2) fib(2) fib(1)
         /  \    /  \   /  \
      fib(2) fib(1)... ... ...
```

**Memoised: O(n) time, O(n) space**

```c
int fib_memo(int n, int *cache) {
    if (n <= 1) return n;
    if (cache[n] != -1) return cache[n];
    cache[n] = fib_memo(n - 1, cache) + fib_memo(n - 2, cache);
    return cache[n];
}
```

**Tabulated: O(n) time, O(1) space**

```c
int fib_tab(int n) {
    if (n <= 1) return n;
    int prev2 = 0, prev1 = 1;
    for (int i = 2; i <= n; i++) {
        int curr = prev1 + prev2;
        prev2 = prev1;
        prev1 = curr;
    }
    return prev1;
}
```

#### 4. 0/1 Knapsack Problem

**Problem:** Given n items with weights w_i and values v_i, and capacity W, select items (entirely or not at all) to maximise value without exceeding capacity.

**State Definition:** dp[i][w] = maximum value achievable using items 1..i with capacity w

**Recurrence:**
```
dp[i][w] = max(dp[i-1][w],                    // Don't take item i
               dp[i-1][w - w_i] + v_i)        // Take item i (if w_i ≤ w)
```

**Table Construction:**

```
Items: {w=2,v=3}, {w=3,v=4}, {w=4,v=5}, {w=5,v=6}
Capacity W = 5

    w→  0   1   2   3   4   5
i↓  ┌───┬───┬───┬───┬───┬───┐
0   │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │
    ├───┼───┼───┼───┼───┼───┤
1   │ 0 │ 0 │ 3 │ 3 │ 3 │ 3 │  item 1: w=2, v=3
    ├───┼───┼───┼───┼───┼───┤
2   │ 0 │ 0 │ 3 │ 4 │ 4 │ 7 │  item 2: w=3, v=4
    ├───┼───┼───┼───┼───┼───┤
3   │ 0 │ 0 │ 3 │ 4 │ 5 │ 7 │  item 3: w=4, v=5
    ├───┼───┼───┼───┼───┼───┤
4   │ 0 │ 0 │ 3 │ 4 │ 5 │ 7 │  item 4: w=5, v=6
    └───┴───┴───┴───┴───┴───┘
    
Answer: dp[4][5] = 7 (items 1 and 2)
```

#### 5. Longest Common Subsequence (LCS)

**Problem:** Given two sequences X and Y, find the longest subsequence common to both.

**Note:** A subsequence maintains relative order but need not be contiguous.

**State Definition:** dp[i][j] = length of LCS of X[1..i] and Y[1..j]

**Recurrence:**
```
dp[i][j] = dp[i-1][j-1] + 1           if X[i] == Y[j]
         = max(dp[i-1][j], dp[i][j-1]) otherwise
```

**Example:**
```
X = "ABCBDAB"
Y = "BDCABA"

LCS = "BCBA" (length 4)
```

---

## 🏭 Industrial Applications

### Database Query Optimisation (Divide and Conquer)

Modern database systems use divide-and-conquer for merge joins:

```c
// External merge sort for large datasets
void external_merge_sort(FILE *input, FILE *output, size_t memory_limit) {
    // Phase 1: Create sorted runs that fit in memory
    // Phase 2: Merge runs using k-way merge
}
```

### File Compression (Huffman Coding)

Huffman coding forms the basis of widely-used compression formats:

```c
// Used in: ZIP, gzip, PNG, JPEG, MP3
HuffmanTree *build_compression_tree(const char *data, size_t length) {
    int freq[256] = {0};
    for (size_t i = 0; i < length; i++)
        freq[(unsigned char)data[i]]++;
    return huffman_build(freq);
}
```

### Network Routing (Greedy Algorithms)

Shortest path algorithms in network routing:

```c
// Dijkstra's algorithm (greedy) for routing tables
void compute_routing_table(Graph *network, int source) {
    // Greedy selection of minimum-distance vertex
    // Update distances to neighbours
}
```

### Bioinformatics (Dynamic Programming)

DNA sequence alignment using LCS-based algorithms:

```c
// Smith-Waterman for local sequence alignment
int smith_waterman(const char *seq1, const char *seq2) {
    // DP table with gap penalties
    // Find local optimal alignment
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Sorting Algorithm Implementation and Analysis

Implement and compare multiple sorting algorithms with performance instrumentation.

**Requirements:**
- Implement MergeSort with auxiliary array
- Implement QuickSort with three pivot selection strategies (first, median-of-three, random)
- Implement ShellSort with Hibbard gaps (2^k - 1)
- Add comparison and swap counters to each algorithm
- Generate performance statistics for various input distributions

**File:** `src/exercise1.c`

### Exercise 2: Greedy vs Dynamic Programming

Explore the boundary between greedy and dynamic programming approaches.

**Requirements:**
- Implement greedy coin change algorithm
- Implement DP coin change algorithm
- Find and demonstrate cases where greedy fails
- Implement activity selection with weighted activities
- Implement longest increasing subsequence using DP

**File:** `src/exercise2.c`

---

## 🔧 Build & Run

```bash
# Compile all sources
make

# Run all executables
make run

# Run specific demo
make run-example

# Run automated tests
make test

# Check for memory leaks
make valgrind

# Clean build artifacts
make clean

# Show help
make help
```

---

## 📁 Directory Structure

```
15-algorithmic-paradigms/
├── README.md                 # This file
├── Makefile                  # Build automation
├── slides/
│   ├── presentation-week15.html      # Main lecture slides (~40 slides)
│   └── presentation-comparativ.html  # C vs Python comparison
├── src/
│   ├── example1.c            # Complete working demonstrations
│   ├── exercise1.c           # Sorting algorithms exercise (TODO)
│   └── exercise2.c           # Greedy vs DP exercise (TODO)
├── data/
│   ├── sorting_data.txt      # Sample data for sorting
│   ├── knapsack_items.txt    # Knapsack problem instances
│   └── huffman_text.txt      # Text for Huffman encoding
├── tests/
│   ├── test1_input.txt       # Test input for exercise 1
│   ├── test1_expected.txt    # Expected output for exercise 1
│   ├── test2_input.txt       # Test input for exercise 2
│   └── test2_expected.txt    # Expected output for exercise 2
├── teme/
│   ├── homework-requirements.md  # Two homework assignments
│   └── homework-extended.md      # Five bonus challenges
└── solution/
    ├── exercise1_sol.c       # Solution for exercise 1
    ├── exercise2_sol.c       # Solution for exercise 2
    ├── homework1_sol.c       # Homework 1 solution
    └── homework2_sol.c       # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential

- **CLRS** (Cormen, Leiserson, Rivest, Stein) — *Introduction to Algorithms*, 4th Edition
  - Chapter 4: Divide-and-Conquer
  - Chapter 15: Dynamic Programming
  - Chapter 16: Greedy Algorithms

- **Sedgewick & Wayne** — *Algorithms*, 4th Edition
  - Chapter 2: Sorting
  - Section 4.4: Shortest Paths

### Advanced

- **Knuth** — *The Art of Computer Programming*, Volume 3: Sorting and Searching
- **Bellman** — *Dynamic Programming* (1957, the original treatise)
- **Hoare** — *Quicksort* (Computer Journal, 1962)

### Online Resources

- [VisuAlgo](https://visualgo.net/en/sorting) — Sorting algorithm visualisations
- [MIT OpenCourseWare 6.006](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-fall-2011/) — Introduction to Algorithms
- [Brilliant.org](https://brilliant.org/wiki/master-theorem/) — Master Theorem explanation

---

## ✅ Self-Assessment Checklist

After completing this lab, verify you can:

- [ ] Identify which algorithmic paradigm suits a given problem
- [ ] Write the Master Theorem and apply it to recurrences
- [ ] Implement MergeSort with O(n) auxiliary space
- [ ] Implement QuickSort with randomised pivot selection
- [ ] Prove correctness of the activity selection greedy approach
- [ ] Construct a Huffman tree from character frequencies
- [ ] Convert a naive recursive solution to memoised form
- [ ] Fill a 0/1 Knapsack DP table and reconstruct the solution
- [ ] Implement LCS and recover the actual subsequence
- [ ] Analyse time and space complexity of all covered algorithms

---

## 💼 Interview Preparation

Common questions about algorithmic paradigms:

1. **Explain** the difference between MergeSort and QuickSort. When would you prefer one over the other?

2. **Implement** a function to find the k-th smallest element in an unsorted array in expected O(n) time.

3. **Prove** that the greedy activity selection algorithm yields an optimal solution.

4. **Design** a DP solution for the coin change problem. What is the time and space complexity?

5. **Compare** top-down (memoisation) vs bottom-up (tabulation) dynamic programming. What are the trade-offs?

---

## 🔗 Next Week Preview

**Week 16: Advanced Graph Algorithms & Backtracking**
- Minimum Spanning Trees (Kruskal and Prim)
- Topological Sort and Strongly Connected Components
- Backtracking paradigm and state-space exploration
- Classic problems: N-Queens, Sudoku, Graph Colouring

---

*Laboratory materials prepared for ATP Course, Academy of Economic Studies - CSIE Bucharest*
