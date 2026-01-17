# Week 14 extended challenges

## Rationale

The extended challenges are optional research-flavoured tasks intended for students who wish to explore beyond the examinable core. They reward depth of reasoning, experimental discipline and clear communication. Each correctly completed challenge adds up to **+10 bonus points**.

Bonus points are only awarded if both mandatory homeworks achieve at least 35/50. This rule exists to prevent a situation in which an advanced extension is used to compensate for missing fundamentals.

The challenges below are written so that a strong solution can be described, tested and defended within a single file if required although multi-file submissions are encouraged if the submission system permits it.

---

## Challenge 1: Cache-oblivious matrix multiplication

### Background and motivation

Naive matrix multiplication is `Θ(n³)` and typically uses loop nests of the form `for i`, `for j`, `for k`. On modern architectures the dominant cost is often memory traffic rather than arithmetic. Cache-oblivious algorithms attempt to reduce cache misses without requiring explicit cache parameters. The classical approach is to use a recursive divide-and-conquer strategy that implicitly creates blocks that fit into caches at multiple levels.

### Required deliverables

1. A naive baseline implementation.
2. A recursive cache-oblivious implementation that uses submatrix views.
3. A benchmark harness that compares timings for sizes `64×64`, `256×256`, `512×512` and `1024×1024`.
4. Evidence of improved cache behaviour via `perf stat` or an equivalent tool.

### Suggested structure

```c
typedef struct {
    double* data;
    int rows;
    int cols;
    int stride;
} Matrix;

void mm_naive(const Matrix* A, const Matrix* B, Matrix* C);
void mm_recursive(const Matrix* A, const Matrix* B, Matrix* C);

static void mm_rec(const Matrix* A, const Matrix* B, Matrix* C,
                   int ar, int ac,
                   int br, int bc,
                   int cr, int cc,
                   int size);
```

### Pseudocode sketch

```
MM_REC(A, B, C, size):
    if size ≤ threshold:
        multiply naively
        return
    split size into half = size/2
    recursively multiply the 8 subproblems
    accumulate partial results into C
```

A correct and efficient implementation must treat the matrix as a strided array. Copying submatrices defeats the cache-oblivious intent.

### Evaluation criteria

- correctness for random inputs
- scaling behaviour as n increases
- evidence of reduced cache miss counts for larger matrices

---

## Challenge 2: A* pathfinding with heuristics

### Background and motivation

A* generalises Dijkstra by introducing an admissible heuristic `h(n)` that estimates the remaining cost to the goal. When the heuristic is consistent A* is optimally efficient among algorithms that use the same information. The challenge is to implement A* and to demonstrate how different heuristics affect the number of expanded nodes.

### Requirements

1. Implement A* with a binary heap priority queue.
2. Support multiple heuristics: Manhattan, Euclidean and Chebyshev.
3. Load a grid map from file and support obstacles.
4. Compare against Dijkstra on the same maps.
5. Report nodes expanded, path length and runtime.

### Suggested data structures

```c
typedef struct { int x, y; } Point;

typedef struct {
    int** grid;
    int width;
    int height;
    Point start;
    Point goal;
} GridMap;

typedef double (*Heuristic)(Point a, Point b);
```

### Pseudocode sketch

```
A_STAR(start, goal):
    open ← priority queue ordered by f = g + h
    g[start] ← 0
    parent[start] ← none
    push start with f(start)

    while open not empty:
        u ← pop min f
        if u = goal: return reconstruct
        for each neighbour v of u:
            tentative ← g[u] + cost(u,v)
            if tentative < g[v]:
                g[v] ← tentative
                parent[v] ← u
                push or decrease-key v with f(v)
```

### Evaluation criteria

A strong submission will include both qualitative and quantitative comparisons, including a discussion of heuristic admissibility and consistency.

---

## Challenge 3: Persistent data structures in C

### Background

Persistence means that updates create new versions without destroying the old ones. Functional languages provide this naturally but C does not. Implementing even a modest persistent structure forces careful reasoning about sharing, reference counting and deallocation.

### Task

Implement a persistent stack or persistent binary search tree where each update returns a new root and shares unchanged substructures.

### Hints

- Use reference counts or a region allocator.
- Ensure that memory reclamation is deterministic.

### Evaluation criteria

- correctness across versions
- structural sharing evidence (do not copy entire structures)
- clear ownership model

---

## Challenge 4: Empirical complexity classification

### Background

Many students can state complexity classes but fewer can classify an unknown implementation empirically. This challenge asks for a small tool that attempts to infer a complexity class from timing measurements.

### Task

Given timing data pairs `(n, t(n))`, infer the most plausible class among a fixed set: `O(1)`, `O(log n)`, `O(n)`, `O(n log n)` and `O(n²)`.

### Suggested approach

1. Fit each candidate model by scaling.
2. Compute normalised residual errors.
3. Use a simple bootstrap or jackknife resampling to test stability.

### Evaluation criteria

- a clear explanation of assumptions
- reproducibility of fits
- sensible failure behaviour when data is noisy

---

## Suggested reading (optional)

The following sources are suitable starting points for the themes above.

- Frigo, M., Leiserson, C. E., Prokop, H., & Ramachandran, S. (1999). Cache-oblivious algorithms. *Proceedings of the 40th Annual Symposium on Foundations of Computer Science*. https://doi.org/10.1109/SFFCS.1999.814600
- Hart, P. E., Nilsson, N. J., & Raphael, B. (1968). A formal basis for the heuristic determination of minimum cost paths. *IEEE Transactions on Systems Science and Cybernetics, 4*(2), 100–107. https://doi.org/10.1109/TSSC.1968.300136


## Additional methodological notes for the extended challenges

The extended challenges are explicitly research flavoured. The intended learning outcome is not merely to produce code that runs but to practise building a defensible empirical argument that connects theoretical expectations with measured behaviour.

### General experimental protocol

1. **Define a hypothesis** in one or two sentences. Example: “The recursive cache-oblivious multiplication reduces cache misses sufficiently to outperform the naive triple loop for `n≥512` on the target machine.”
2. **Control confounders**: use fixed seeds, isolate the machine (close background workloads), pin CPU frequency if possible.
3. **Report uncertainty**: for each `n` compute mean and standard deviation over repeated runs.
4. **Interpret failures**: if the hypothesis is not supported, explain why. “No speed-up” is a valid outcome if well argued.

### Challenge 1 extended discussion: cache behaviour as a cost model

The naive matrix multiplication

```
for i in 0..n-1:
    for j in 0..n-1:
        for k in 0..n-1:
            C[i][j] += A[i][k] * B[k][j]
```

has `Θ(n³)` arithmetic operations but its performance is often limited by memory bandwidth rather than by arithmetic throughput. The cache-oblivious approach replaces the flat loops with a recursion that increases temporal locality by operating on submatrices that fit in cache.

A standard recursive decomposition is:

```
MM(A, B, C, n):
    if n <= n0:
        NAIVE_MM(A, B, C, n)
        return
    split A, B, C into four (n/2)x(n/2) blocks
    C11 += A11*B11 + A12*B21
    C12 += A11*B12 + A12*B22
    C21 += A21*B11 + A22*B21
    C22 += A21*B12 + A22*B22
```

The algorithm remains `Θ(n³)` in arithmetic complexity but may reduce cache misses asymptotically under the ideal cache model. Your report should articulate this separation between arithmetic complexity and memory complexity.

### Challenge 2 extended discussion: heuristics as informed search

A* can be understood as Dijkstra with an admissible bias. If `h(v)` never overestimates the true remaining cost to the goal, then A* is guaranteed to return an optimal path while potentially expanding far fewer vertices.

The key comparison quantities are:

- nodes expanded
- peak size of the frontier
- wall-clock time under identical priority queue implementations

You should also explicitly state whether your grid allows diagonal motion, how you encode obstacles and whether costs are uniform.

### Challenge 3: Minimum cut as a probabilistic algorithm (Hard)

Implement Karger’s random contraction algorithm for the global minimum cut of an undirected multigraph.

Minimum requirements:

1. Graph representation that supports parallel edges
2. Random contraction repeated `O(n² log n)` times to drive down failure probability
3. Empirical verification on small graphs with known min cuts

Pseudocode sketch:

```
KARGER_MIN_CUT(G):
    best := ∞
    repeat R times:
        H := copy(G)
        while |V(H)| > 2:
            choose random edge (u, v)
            contract u and v into a supernode
            remove self-loops
        best := min(best, number_of_edges_between_remaining_supernodes)
    return best
```

A strong write-up will include a discussion of the success probability and why repetition is required.

### Challenge 4: Suffix array construction and pattern queries (Medium)

Construct a suffix array for a given string and support pattern queries via binary search.

Minimum requirements:

- construction via doubling method `Θ(n log n)` or via an alternative well justified method
- pattern query in `Θ(m log n)` where `m` is pattern length
- demonstration on both random text and structured text (repetitions)

You should report memory usage because suffix arrays trade construction complexity against memory footprint.

## References

| Topic | Reference |
|---|---|
| Cache-oblivious algorithms | Frigo, M., Leiserson, C. E., Prokop, H. and Ramachandran, S. (1999). Cache-oblivious algorithms. *40th Annual Symposium on Foundations of Computer Science*, 285–297. https://doi.org/10.1109/SFFCS.1999.814600 |
| A* optimality conditions | Hart, P. E., Nilsson, N. J. and Raphael, B. (1968). A formal basis for the heuristic determination of minimum cost paths. *IEEE Transactions on Systems Science and Cybernetics, 4*(2), 100–107. https://doi.org/10.1109/TSSC.1968.300136 |
| Random contraction for min cut | Karger, D. R. (1993). Global min-cuts in RNC and other ramifications of a simple min-cut algorithm. *Proceedings of the Fourth Annual ACM-SIAM Symposium on Discrete Algorithms*, 21–30. (Conference paper, no DOI) |


---

## ⭐ Challenge 4: Approximate membership with Bloom filters (Difficulty: Medium)

### Description

Implement a Bloom filter for approximate set membership and integrate it into an application-level scenario, such as deduplicating tokens from a text stream or caching visited states in a search procedure.

### Theoretical background

A Bloom filter represents a set using a bit array of length `m` and `k` independent hash functions. On insertion the filter sets `k` bits. On query the filter returns “possibly present” if all `k` bits are set and “definitely absent” otherwise. False positives are possible but false negatives are not, provided the implementation is correct.

For `n` inserted elements the approximate false positive probability is:

`p ≈ (1 - e^{-kn/m})^k`

This equation can be used to select `m` and `k` subject to a target `p` and a chosen `n`.

### Required components

1. Bit array representation using `uint8_t` or `uint64_t` blocks.
2. At least two independent hash constructions. A common approach is double hashing: `h_i(x) = h1(x) + i*h2(x)`.
3. A driver that inserts `n` strings then queries a mixed stream of present and absent keys.
4. An empirical estimate of the false positive rate compared with the theoretical approximation.

### Pseudocode

```
BLOOM_INIT(m, k):
    bits <- array[0..m-1] initialised to 0

BLOOM_ADD(x):
    for i in 0..k-1:
        idx <- (h1(x) + i*h2(x)) mod m
        bits[idx] <- 1

BLOOM_QUERY(x):
    for i in 0..k-1:
        idx <- (h1(x) + i*h2(x)) mod m
        if bits[idx] == 0:
            return DEFINITELY_ABSENT
    return POSSIBLY_PRESENT
```

### Assessment focus

- Correctness of bit addressing and hashing
- Justification of parameter selection using `m`, `k` and `n`
- Empirical methodology for estimating `p`

---

## Reporting expectations for optional challenges

Optional challenge submissions should include a short methodological appendix describing:

- the experimental environment (CPU model, OS, compiler, optimisation flags)
- the workload generation process and any fixed seeds
- the metrics collected and how they were computed

Reports that are methodologically explicit are easier to evaluate fairly and they demonstrate the scientific discipline expected from advanced work.
