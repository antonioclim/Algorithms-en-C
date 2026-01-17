# Week 14 Homework: Advanced Topics and Comprehensive Review

## Purpose and expected learning outcomes

This homework is designed as a capstone exercise. It assumes that students can implement basic algorithms and data structures and instead evaluates whether those components can be integrated into a coherent command-line tool with explicit interfaces, deterministic behaviour and empirically defensible measurements.

By completing the assignment a student should be able to:

1. Design a small but principled library interface in C11.
2. Implement a collection of algorithms behind that interface using function pointers.
3. Justify complexity claims both theoretically and empirically.
4. Produce reproducible outputs suitable for automated assessment.
5. Demonstrate memory safety by construction rather than by luck.

---

## General information

- **Deadline:** end of the examination period
- **Total points:** 100 (contributes 10% towards the final grade)
- **Language:** C (C11)
- **Compiler model:** GCC with `-Wall -Wextra -std=c11`
- **Submission:** one `.c` file per homework via the university portal

### Assessment principles

The rubric rewards correctness, clarity and robustness. A solution that is correct for one input but fragile under small perturbations should not be considered complete.

When in doubt prefer:

- explicit precondition checks
- deterministic iteration order
- bounded resource usage
- descriptive error messages written to stderr

---

## Homework 1: Algorithm toolkit library (50 points)

### High-level brief

Design and implement a unified algorithm toolkit that exposes sorting, searching, graph algorithms and at least one dynamic programming algorithm through a command-driven interface. The programme should be usable both as a tool (processing files) and as a self-demonstration (generating its own data).

### Functional requirements and marking rubric

| Requirement family | Points | Assessment focus |
|---|---:|---|
| Core data structures | 15 | representation, invariants, memory ownership |
| Algorithm suite | 20 | correctness, asymptotic behaviour, edge cases |
| Interface and I/O | 15 | deterministic output, usable CLI, error handling |

#### Core data structures (15)

1. Generic dynamic array container with metadata (5)
   - amortised growth strategy, typically doubling
   - explicit element size and capacity

2. Adjacency list graph representation (5)
   - directed and undirected variants
   - clear ownership of adjacency nodes

3. Hash table with chaining, minimum 97 buckets (5)
   - stable hash function
   - collision resolution via linked lists

#### Algorithm suite (20)

- at least three sorting algorithms selected by name via a function pointer table (6)
- binary search implemented iteratively and recursively (4)
- Dijkstra shortest paths (5)
- one dynamic programming solution (5) such as LCS, knapsack or edit distance

#### Interface and I/O (15)

- command-line parsing for operation selection (5)
- file I/O for loading and saving data (5)
- formatted output including timing information (5)

### Required signatures

```c
typedef int (*Comparator)(const void*, const void*);

typedef void (*SortFunction)(void* arr, size_t n, size_t size, Comparator cmp);

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

### Required behaviours

1. The programme must fail gracefully on malformed input files.
2. Sorting and searching must use the provided comparator consistently.
3. Dijkstra must reject negative edges explicitly.
4. Output must be deterministic for a fixed seed and fixed input.

### Recommended architecture

A robust structure for a single-file submission is:

- type definitions and private helpers
- data structure implementations
- algorithm implementations
- CLI parser and dispatch
- main

Avoid interleaving unrelated concerns. In C a single file can still be structured.

### Pseudocode exemplars

#### Dynamic array append

```
ARRAY_APPEND(arr, elem):
    if arr.count = arr.capacity:
        new_capacity ← max(1, 2*arr.capacity)
        new_data ← realloc(arr.data, new_capacity * arr.elem_size)
        if new_data = NULL: return error
        arr.data ← new_data
        arr.capacity ← new_capacity
    memcpy(arr.data + arr.count*arr.elem_size, elem, arr.elem_size)
    arr.count ← arr.count + 1
    return success
```

#### Comparator-aware quicksort skeleton

```
QS(A, low, high, cmp):
    if low < high:
        p ← PARTITION(A, low, high, cmp)
        QS(A, low, p-1, cmp)
        QS(A, p+1, high, cmp)
```

The key in generic sorting is not the recursion. It is the pointer arithmetic and the disciplined use of `cmp`.

### Example usage

```bash
./homework1 --sort quicksort --input numbers.txt --output sorted.txt
./homework1 --search 42 --input sorted.txt
./homework1 --graph roads.txt --dijkstra 0
./homework1 --demo --seed 42
```

### Input formats

Number file format (one integer per line):

```
42
17
85
3
99
```

Graph file format (first line: V E, then E edges):

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

### Output contract suggestions

For automated assessment produce output that is stable for fixed inputs:

- avoid printing memory addresses
- avoid timing outputs in mandatory transcripts unless explicitly requested
- sort vertex and edge listings deterministically

---

## Homework 2: Performance analysis framework (50 points)

### High-level brief

Implement a benchmarking framework that empirically analyses algorithm complexity. The goal is methodological literacy. A benchmark is only as credible as its experimental design.

### Functional requirements and rubric

| Requirement family | Points | Assessment focus |
|---|---:|---|
| Data generation | 12 | distributional control, reproducibility |
| Timing infrastructure | 13 | correct clocks, statistics |
| Complexity estimation | 15 | inference discipline, model fit |
| Output and visualisation | 10 | usable exports, clear summaries |

### Data generation (12)

- random array generator with configurable seed (3)
- sorted, reverse-sorted and partially sorted generators (3)
- graph generator with multiple regimes (3)
- explicit reproducibility (3)

### Timing infrastructure (13)

- use `clock_gettime()` with `CLOCK_MONOTONIC` where available (4)
- multiple runs with outlier elimination (4)
- compute mean, median and standard deviation (5)

### Complexity estimation (15)

A plausible minimal approach is:

1. Choose a set of candidate classes: `O(1)`, `O(log n)`, `O(n)`, `O(n log n)`, `O(n²)`.
2. For each candidate compute a normalised error against observed timings.
3. Choose the candidate with the lowest error subject to stability under resampling.

The assignment encourages ratio tests because they are easy to explain.

### Output and visualisation (10)

- CSV export suitable for plotting in R, Python or Excel (3)
- ASCII charts for terminal inspection (4)
- summary report with pass/fail criteria (3)

### Practical guidance

- Warm up caches by running at least one untimed trial.
- Pin the seed and document it.
- Avoid I/O in timed regions.
- Report the clock used and its resolution.

---

## Submission checklist

1. The programme compiles with `-Wall -Wextra -std=c11` and no warnings.
2. All allocated memory is freed on all code paths.
3. The tool prints a `--help` message that describes arguments.
4. Deterministic behaviour: fixed seed implies fixed outputs.
5. The report explains methodology, not only results.

---

## Suggested reading (optional)

The following are established references that discuss algorithms and performance reasoning. They are not required but they provide authoritative context.

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to algorithms* (3rd ed). MIT Press. https://doi.org/10.7551/mitpress/9780262033848.001.0001
- Knuth, D. E. (1998). *The art of computer programming, volume 3: Sorting and searching* (2nd ed). Addison-Wesley. https://doi.org/10.1002/9781118032703


## Implementation and assessment guidance

This section is not an additional set of requirements. It is a clarification of what will be examined in practice when a submission is assessed.

### Architectural expectations

A strong submission will exhibit an explicit separation between:

- *representation*: how data are stored (dynamic arrays, adjacency lists, hash tables)
- *interfaces*: stable function signatures and clear contracts
- *algorithms*: implementations that assume the interface contract and state invariants
- *I/O and reporting*: file parsing, formatting and error handling

The mark scheme rewards this separation because it supports systematic debugging and it reduces the probability of hidden aliasing and memory ownership errors.

### Robustness requirements

Even when inputs are well formed, robustness should be engineered rather than assumed.

- Validate sizes before allocation and check every allocation result.
- Ensure that all ownership transfers are explicit. If a function returns a pointer that must be freed, document it.
- Treat integer overflow as a correctness bug. When computing `dist[u] + w` for Dijkstra, guard against overflow.
- Prefer deterministic traversal order in all graph algorithms. When multiple neighbours are available, iterate in ascending vertex order.

### Required algorithms: canonical pseudocode

The following pseudocode captures the minimum expected logical structure. You may implement variants, but your report must state which variant you selected and why.

#### Binary search (iterative)

```
BINARY_SEARCH(A[0..n-1], key):
    lo := 0
    hi := n-1
    while lo <= hi:
        mid := lo + (hi-lo)//2
        if A[mid] == key: return mid
        if A[mid] < key: lo := mid+1
        else: hi := mid-1
    return NOT_FOUND
```

#### Dijkstra (priority queue variant)

```
DIJKSTRA(G, s):
    for each v in V:
        dist[v] := ∞
        parent[v] := NIL
    dist[s] := 0
    PQ := min-priority-queue keyed by dist
    PQ.push(s)
    while PQ not empty:
        u := PQ.pop_min()
        for each edge (u, v, w) outgoing:
            if dist[u] + w < dist[v]:
                dist[v] := dist[u] + w
                parent[v] := u
                PQ.decrease_key(v, dist[v]) or PQ.push(v)
    return dist, parent
```

#### Longest common subsequence (dynamic programming)

```
LCS_LENGTH(X[1..m], Y[1..n]):
    create table L[0..m][0..n]
    for i := 0..m: L[i][0] := 0
    for j := 0..n: L[0][j] := 0
    for i := 1..m:
        for j := 1..n:
            if X[i] == Y[j]:
                L[i][j] := L[i-1][j-1] + 1
            else:
                L[i][j] := max(L[i-1][j], L[i][j-1])
    return L[m][n]
```

### Marking rubric clarifications

- Correctness is necessary but not sufficient for high marks. An implementation that passes a narrow set of tests but lacks clear contracts and memory safety will be penalised.
- Complexity claims must be justified. If you state that an operation is `Θ(n log n)` then you must describe the dominant step and the cost of that step.
- Benchmark results must be reproducible. If your programme prints timings but uses a changing seed without reporting it, results are not scientifically interpretable.

### Academic integrity note

You may consult external references to learn, but the submitted code must be authored by you and must be attributable. When you adapt an idea, credit the source in a comment. This is treated as good scholarly practice rather than as a liability.

## References

| Topic | Reference |
|---|---|
| Quicksort original paper | Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–15. https://doi.org/10.1093/comjnl/5.1.10 |
| Single-source shortest paths | Dijkstra, E. W. (1959). A note on two problems in connexion with graphs. *Numerische Mathematik, 1*, 269–271. https://doi.org/10.1007/BF01386390 |
| Dynamic programming foundations | Bellman, R. (1957). *Dynamic Programming*. Princeton University Press. (Monograph, no DOI) |


## Submission hygiene and reproducibility checklist

Before submission you should treat your programme as an artefact that will be built and executed on an environment that you do not control.

1. **Build determinism:** ensure that `gcc -Wall -Wextra -std=c11` produces no warnings. Warnings will be treated as defects because they are frequently symptoms of undefined behaviour.
2. **Input defensiveness:** validate headers, bounds and file operations. Reject malformed inputs early with clear diagnostics.
3. **Ownership clarity:** each allocation must have exactly one responsible owner. Pair every `malloc` or `calloc` with a unique and reachable `free`.
4. **Transcript stability:** ensure that results are deterministic for a fixed seed and that output formatting is invariant across runs.
5. **Complexity narrative:** for each algorithm, state the representation assumptions that justify your complexity claim.

If you follow this checklist, most high-impact marking deductions can be avoided.
