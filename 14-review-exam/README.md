# Week 14: Advanced Topics and Comprehensive Review

## Abstract

Week 14 is framed as a consolidation week and as an examination-calibrated rehearsal. The emphasis is not merely on implementing canonical algorithms but on demonstrating that they can be composed, tested and reasoned about under constraints that mirror real assessment conditions: deterministic I/O transcripts, explicit invariants, principled memory ownership and carefully delimited side effects.

The repository therefore provides three artefacts that together function as a miniature portfolio:

- `src/example1.c` is an integrated demonstration programme (“Algorithm Portfolio Manager”) that illustrates how a heterogeneous algorithm set can be registered, selected and benchmarked using function pointers and lightweight data structures.
- `src/exercise1.c` implements a small sorting portfolio with two operational profiles: a strict regression profile driven by stdin and a benchmarking profile driven by generated data.
- `src/exercise2.c` implements a graph analyser with a regression profile that emits a stable transcript and an interactive profile that produces a richer report, including file export.

Both exercises are engineered to be testable via `diff`. This is not cosmetic. A programme that is correct but not deterministic is frequently unmarkable in practice and often fails continuous integration pipelines.

## Table of contents

1. Repository structure
2. Build, run and test workflows
3. Exercise 1: sorting portfolio
4. Exercise 2: graph analyser
5. Example 1: architecture notes
6. Algorithmic dossiers and pseudocode
7. Cross-language correspondences
8. Testing methodology and failure modes
9. Appendices

---

## Repository structure

```
14-review-exam/
├─ Makefile
├─ README.md
├─ src/
│  ├─ example1.c
│  ├─ exercise1.c
│  └─ exercise2.c
├─ data/
│  └─ graph_sample.txt
├─ tests/
│  ├─ test1_input.txt
│  ├─ test1_expected.txt
│  ├─ test2_input.txt
│  └─ test2_expected.txt
└─ teme/
   ├─ homework-requirements.md
   └─ homework-extended.md
```

The directory `solution/` is included as an instructor reference. For this week it also provides regression-compatible binaries so that `make test-solutions` can serve as a calibration tool.

---

## Build, run and test workflows

### Compilation model

All C code targets the C11 standard and is compiled with warnings enabled. The Makefile uses a single compilation unit per programme for pedagogical clarity. In larger systems one would normally split code into translation units and introduce headers with explicit interfaces.

### Canonical commands

- Build student exercises

```bash
make
```

- Run the integrated demonstration programme

```bash
make run
```

- Run regression tests (strict, failing on mismatch)

```bash
make test
```

- Run regression tests for solution binaries

```bash
make test-solutions
```

### Deterministic regression transcripts

Both `exercise1` and `exercise2` implement a quiet profile that is triggered when output is not a terminal.

- In `exercise1` regression mode is selected when stdin is not a terminal, which is the standard situation when a test harness redirects stdin from a file.
- In `exercise2` regression mode is selected when stdout is not a terminal, which is the standard situation when a harness captures programme output.

The intent is to separate human-facing narration from machine-facing transcripts. Both have value but they should not be conflated.

---

## Exercise 1: sorting portfolio

### Behavioural specification

#### Regression profile

Input format:

- First token: integer `n` (number of elements)
- Next `n` tokens: integers

Output format:

1. A single line containing the sorted sequence in non-decreasing order
2. Three lines, one per algorithm, each in the form `AlgorithmName: PASSED` or `AlgorithmName: FAILED`

This profile is designed to be stable under `diff` and to support automated marking.

#### Benchmark profile

When run interactively the programme benchmarks three algorithms across a fixed set of sizes and writes a CSV file (`benchmark_output.csv`). The benchmark is deliberately modest because the intent is to practise methodology rather than to produce publishable microbenchmark results.

### Algorithmic inventory

1. **Selection sort**: a quadratic baseline with constant auxiliary space.
2. **Quicksort (Lomuto partition)**: average-case `Θ(n log n)` with recursion depth dependent on pivot quality.
3. **Merge sort**: guaranteed `Θ(n log n)` time with `Θ(n)` auxiliary storage.

### Data structure and control architecture

The programme uses a function pointer array to store and iterate over algorithm implementations. This achieves two outcomes.

- It eliminates repetitive code and encourages an interface-first style.
- It makes it straightforward to introduce an additional algorithm (heap sort is the obvious extension) without rewriting the control flow.

In C this is a standard technique for implementing strategy selection, callbacks and dispatch tables.

---

## Exercise 2: graph analyser

### Graph model

The analyser loads a weighted undirected graph from a file containing an edge list. Internally it stores the graph in an adjacency matrix `W` where `W[u][v] = 0` denotes absence of an edge and `W[u][v] > 0` denotes the weight.

This representation is intentionally simple.

- It provides deterministic neighbour iteration by scanning `v = 0..V-1`.
- It supports constant-time edge lookup.
- It has `Θ(V²)` memory which is suboptimal for sparse graphs but pedagogically convenient.

### Output obligations

#### Regression transcript

The transcript printed in quiet mode is:

1. A three-line header with vertex count, edge count, density and component count
2. BFS order from source 0
3. DFS order from source 0
4. Dijkstra single-source distances from source 0

The formatting is fixed to match the test files.

#### Interactive report

The interactive profile prints a richer narrative and writes a report (`graph_analysis.txt`) to disk.

---

## Example 1: architecture notes

`src/example1.c` is not a marking target. It is a reference implementation intended to show how semester topics can be combined into a coherent programme structure.

### Core patterns demonstrated

- **Function pointer registries**: algorithms are registered with metadata and stored in an array. Lookup is accelerated by a hash table.
- **Chaining hash table**: collisions are resolved via linked lists. This illustrates the difference between average-case and worst-case lookup.
- **Benchmark logging**: results are appended to a linked list which is then used to produce a report.
- **Graph algorithms**: traversal and shortest path computations are incorporated into the same portfolio.
- **Dynamic programming**: multiple Fibonacci implementations are included to demonstrate memoisation, tabulation and space optimisation.

### Why a registry matters

In an examination context students often implement algorithms as isolated functions. A registry forces an additional layer of discipline.

- Each algorithm must conform to a signature.
- Each algorithm must be callable through an indirection boundary.
- Each algorithm must be testable with shared tooling.

This is a small analogue of how real libraries are designed.

---

## Algorithmic dossiers and pseudocode

The following dossiers summarise the algorithms used in this week. Each dossier includes pseudocode, invariants and complexity claims.

### Selection sort

#### Pseudocode

```
SELECTION_SORT(A[0..n-1]):
    for i ← 0 to n-2:
        m ← i
        for j ← i+1 to n-1:
            if A[j] < A[m]:
                m ← j
        swap A[i], A[m]
```

#### Loop invariants

Let `i` be the outer-loop index.

- Invariant I1: After iteration `i-1` the prefix `A[0..i-1]` is sorted.
- Invariant I2: After iteration `i-1` the prefix contains the `i` smallest elements of the original array.

These invariants are sufficient for a standard induction proof of correctness.

#### Complexity

- Comparisons: `(n-1) + (n-2) + ... + 1 = n(n-1)/2 = Θ(n²)`
- Swaps: at most `n-1`
- Auxiliary space: `Θ(1)`

Selection sort is not stable unless the swap is replaced by a stable insertion step.

#### Worked micro-trace

For `A = [64, 25, 12, 22, 11]`:

- `i=0`: minimum in `[64, 25, 12, 22, 11]` is 11, swap with 64
- `i=1`: minimum in `[25, 12, 22, 64]` is 12, swap with 25
- `i=2`: minimum in `[25, 22, 64]` is 22, swap with 25
- `i=3`: minimum in `[25, 64]` is 25, swap no-op

Result: `[11, 12, 22, 25, 64]`

### Quicksort with Lomuto partition

#### Partition specification

Given indices `low` and `high`, partition chooses `A[high]` as pivot and rearranges the subarray `A[low..high]` so that:

- for all `k ∈ [low, p-1]`, `A[k] ≤ pivot`
- `A[p] = pivot`
- for all `k ∈ [p+1, high]`, `A[k] > pivot`

#### Pseudocode

```
PARTITION_LOMUTO(A, low, high):
    pivot ← A[high]
    i ← low - 1
    for j ← low to high-1:
        if A[j] ≤ pivot:
            i ← i + 1
            swap A[i], A[j]
    swap A[i+1], A[high]
    return i + 1

QUICKSORT(A, low, high):
    if low < high:
        p ← PARTITION_LOMUTO(A, low, high)
        QUICKSORT(A, low, p-1)
        QUICKSORT(A, p+1, high)
```

#### Correctness sketch

The key invariant in partition is:

- After processing `j`, the subarray `A[low..i]` contains elements `≤ pivot` and `A[i+1..j]` contains elements `> pivot`.

At termination, swapping the pivot into `i+1` yields the partition property and recursion preserves correctness.

#### Complexity

- Average time: `Θ(n log n)` under mild distributional assumptions
- Worst-case time: `Θ(n²)` for adversarial inputs (already sorted input with pivot as last element is the classic example)
- Auxiliary space: `Θ(log n)` expected call stack depth and `Θ(n)` worst case depth

The programme uses Lomuto because it is compact and easy to reason about. Hoare partition is often faster but is also easier to implement incorrectly.

#### Engineering note

In examination code bases it is sensible to include a guard against stack depth by either randomising pivots or converting the larger recursive branch into a loop. That optimisation is not required here but it is discussed in the homework.

### Merge sort

#### Pseudocode

```
MERGE_SORT(A, left, right):
    if left < right:
        mid ← left + (right-left)/2
        MERGE_SORT(A, left, mid)
        MERGE_SORT(A, mid+1, right)
        MERGE(A, left, mid, right)

MERGE(A, left, mid, right):
    L ← copy of A[left..mid]
    R ← copy of A[mid+1..right]
    i ← 0, j ← 0, k ← left
    while i < |L| and j < |R|:
        if L[i] ≤ R[j]:
            A[k] ← L[i]; i ← i + 1
        else:
            A[k] ← R[j]; j ← j + 1
        k ← k + 1
    copy remaining items of L then remaining items of R into A
```

#### Recurrence and solution

Let `T(n)` be the worst-case running time. Merge sort satisfies:

- `T(n) = 2T(n/2) + Θ(n)`

By the Master theorem (case 2), `T(n) = Θ(n log n)`.

#### Stability

Merge sort is stable if the merge procedure uses `≤` when selecting from the left run. This is explicitly implemented in the code.

---

## Graph algorithms and metrics

### Graph density

For an undirected simple graph with `V` vertices and `E` edges, density is:

`density = 2E / (V(V-1))`

The implementation returns `0.0` for `V < 2`.

### BFS

#### Pseudocode

```
BFS(G, s):
    for each vertex v:
        visited[v] ← false
    Q ← empty queue
    visited[s] ← true
    enqueue(Q, s)
    order ← empty list

    while Q not empty:
        u ← dequeue(Q)
        append(order, u)
        for v ← 0 to V-1:
            if W[u][v] > 0 and visited[v] = false:
                visited[v] ← true
                enqueue(Q, v)
    return order
```

BFS is used both as a traversal method and as a conceptual foundation for unweighted shortest paths. It is important to stress that the weight field is ignored by BFS.

### DFS

#### Pseudocode

```
DFS(G, s):
    for each vertex v:
        visited[v] ← false
    order ← empty list
    VISIT(s)
    return order

VISIT(u):
    visited[u] ← true
    append(order, u)
    for v ← 0 to V-1:
        if W[u][v] > 0 and visited[v] = false:
            VISIT(v)
```

DFS is used for traversal and as the workhorse in connected component counting.

### Connected components

A component counter is a direct application of DFS.

```
COMPONENTS(G):
    visited[v] ← false for all v
    c ← 0
    for v ← 0 to V-1:
        if visited[v] = false:
            VISIT(v)
            c ← c + 1
    return c
```

### Dijkstra

The implementation uses the adjacency matrix variant which has `Θ(V²)` time complexity.

```
DIJKSTRA(G, s):
    dist[v] ← INF for all v
    parent[v] ← -1 for all v
    processed[v] ← false for all v
    dist[s] ← 0

    repeat V times:
        u ← argmin dist[u] where processed[u] = false
        if u = -1: break
        processed[u] ← true

        for v ← 0 to V-1:
            w ← W[u][v]
            if w > 0 and processed[v] = false and dist[u] + w < dist[v]:
                dist[v] ← dist[u] + w
                parent[v] ← u

    return dist, parent
```

Dijkstra requires non-negative edge weights. If negative edges are present Bellman–Ford is the standard alternative.

---

## Cross-language correspondences

The purpose of cross-language correspondence is not to encourage language switching during assessment. It is to make clear which conceptual moves are language independent and which are C specific.

### Sorting

**Python**

```python
xs = [64, 25, 12, 22, 11]
xs_sorted = sorted(xs)
```

In Python `sorted` implements Timsort which is stable and exploits existing runs. It is therefore not comparable as a pure algorithmic baseline.

**C++**

```cpp
std::vector<int> xs{64, 25, 12, 22, 11};
std::sort(xs.begin(), xs.end());
```

`std::sort` is typically introsort (quicksort with heap sort fallback). It has `O(n log n)` worst-case time.

**Java**

```java
int[] xs = {64, 25, 12, 22, 11};
java.util.Arrays.sort(xs);
```

For primitive arrays Java uses a dual-pivot quicksort. For object arrays it uses a stable TimSort variant.

### BFS

**Python**

```python
from collections import deque

def bfs(W, s):
    V = len(W)
    seen = [False]*V
    q = deque([s])
    seen[s] = True
    order = []
    while q:
        u = q.popleft()
        order.append(u)
        for v in range(V):
            if W[u][v] > 0 and not seen[v]:
                seen[v] = True
                q.append(v)
    return order
```

**Java**

```java
Queue<Integer> q = new ArrayDeque<>();
boolean[] seen = new boolean[V];
q.add(s);
seen[s] = true;
while (!q.isEmpty()) {
    int u = q.remove();
    // process u
    for (int v = 0; v < V; v++) {
        if (W[u][v] > 0 && !seen[v]) {
            seen[v] = true;
            q.add(v);
        }
    }
}
```

The conceptual mapping to the C implementation is direct: the queue carries the frontier, `seen` corresponds to `visited` and the adjacency representation determines the neighbour iteration cost.

---

## Testing methodology and failure modes

### What the tests actually guarantee

The provided tests are intentionally narrow. They guarantee:

- The sorting algorithms produce a correct non-decreasing order for a specific input and that all three agree on the output.
- The graph analyser correctly parses the input file, computes density and component count and prints BFS, DFS and Dijkstra outputs with deterministic formatting.

They do not guarantee:

- Performance properties.
- Robustness under malformed input.
- Absence of memory leaks for all code paths.

The intent is to provide a baseline regression harness and to encourage students to extend it.

### Common failure modes

1. **Non-deterministic printing**: extra spaces, trailing spaces or additional banners in regression mode.
2. **Incorrect pivot handling**: off-by-one errors in Lomuto partition leading to out-of-bounds swaps.
3. **Merge buffer mistakes**: allocating incorrect sizes or failing to free temporary arrays.
4. **Graph initialisation errors**: failing to zero the adjacency matrix or leaving `visited` uninitialised.
5. **Overflow in Dijkstra**: adding weights to `INT_MAX` without checking.

### Recommended local extensions

- Add tests with repeated values to confirm stability claims.
- Add a disconnected graph input to check component counting.
- Add an unreachable vertex case to check the `unreachable` output path.

---

## Appendices

### Appendix A: Minimal I/O transcripts

Exercise 1 regression transcript:

```
11 12 22 25 33 45 56 64 78 90
SelectionSort: PASSED
QuickSort: PASSED
MergeSort: PASSED
```

Exercise 2 regression transcript skeleton:

```
Graph: V vertices, E edges
Density: d.ddd
Components: c

BFS from 0: ...
DFS from 0: ...

Dijkstra from vertex 0:
  To 0: 0
  To 1: ...
```

### Appendix B: Complexity cheat sheet

- `Θ(n²)` algorithms become infeasible rapidly. Doubling `n` multiplies time by roughly four.
- `Θ(n log n)` algorithms scale substantially better. Doubling `n` multiplies time by slightly more than two.
- `Θ(2^n)` algorithms are only appropriate for small `n` or when combined with pruning and memoisation.

### Appendix C: When to choose a representation

- Adjacency matrix: good for dense graphs, constant-time edge lookup, deterministic scans, poor for very sparse graphs.
- Adjacency list: good for sparse graphs, `Θ(V+E)` traversals, more complex to implement correctly in C.

These representation choices are not decorative. They are algorithmic decisions that dominate asymptotic costs and real performance.


### Appendix D: Worked trace for Dijkstra on the sample graph

To connect the abstract invariant of Dijkstra’s method with an auditable trace, consider the graph used in `tests/test2_input.txt`:

```
V=5, E=6
0—1 (4)
0—2 (1)
1—2 (2)
1—3 (5)
2—3 (8)
3—4 (3)
```

The algorithm maintains two arrays:

- `dist[v]`: current best known distance from source `s=0` to `v`
- `processed[v]`: whether `v` has been permanently settled

Initialisation:

- `dist[0]=0`
- `dist[v]=∞` for `v≠0`
- all `processed[v]=false`

Iteration 1 (select min unprocessed):

- choose `u=0` since `dist[0]=0` is minimal
- set `processed[0]=true`
- relax neighbours:
  - `dist[1] = min(∞, dist[0]+w(0,1)=0+4)=4`
  - `dist[2] = min(∞, dist[0]+w(0,2)=0+1)=1`

Iteration 2:

- choose `u=2` since `dist[2]=1` is minimal among unprocessed vertices
- set `processed[2]=true`
- relax neighbours:
  - `dist[1] = min(4, dist[2]+w(2,1)=1+2)=3`  (strict improvement)
  - `dist[3] = min(∞, dist[2]+w(2,3)=1+8)=9`

Iteration 3:

- choose `u=1` since `dist[1]=3` is minimal
- set `processed[1]=true`
- relax neighbours:
  - `dist[3] = min(9, dist[1]+w(1,3)=3+5)=8`  (strict improvement)

Iteration 4:

- choose `u=3` since `dist[3]=8` is minimal
- set `processed[3]=true`
- relax neighbours:
  - `dist[4] = min(∞, dist[3]+w(3,4)=8+3)=11`

Iteration 5:

- choose `u=4` and terminate (all vertices processed)

Final distances match the expected transcript:

- `dist = [0, 3, 1, 8, 11]`

The critical invariant is that once a vertex is selected as minimal among unprocessed vertices it cannot later be improved, which relies on non-negative edge weights.

### Appendix E: Pivot selection strategies and their consequences

The implementation uses the Lomuto partition scheme with the last element as pivot. This choice is intentionally transparent: it is easy to prove correct and easy to translate into pseudocode. The trade-off is that adversarial inputs may trigger the quadratic case.

A non-exhaustive pivot taxonomy:

- **Last element pivot (used here)**: simplest, susceptible to sorted or reverse-sorted inputs.
- **First element pivot**: symmetric vulnerability.
- **Middle element pivot**: mild improvement, still predictable.
- **Random pivot**: expected `Θ(n log n)` under broad assumptions, requires a trustworthy RNG and a reproducibility plan.
- **Median-of-three**: pragmatic compromise, reduces the probability of pathological partitions.

If you change the pivot strategy you must preserve the partition invariant. In particular the boundary index must continue to represent “the last index of the ≤ pivot region” while scanning.

### Appendix F: Benchmark methodology as an empirical argument

A benchmark table is not an experiment unless its assumptions are made explicit. For this repository the benchmark mode is intended as a didactic probe rather than a publication-grade evaluation.

Recommended methodological practices:

1. Fix the random seed for reproducibility and record it in the output.
2. Run multiple repetitions and report at least mean and median.
3. Avoid printing during timed sections because I/O dominates algorithmic costs for small `n`.
4. Warm up caches where relevant or discard the first run.
5. Report the machine context (CPU model, compiler version, optimisation flags) when results are compared across machines.

The code measures CPU time via `clock()` because it is standard C. For higher resolution and better isolation from scheduling noise, use `clock_gettime(CLOCK_MONOTONIC, ...)`.

### Further reading

The following sources are well suited to a week 14 revision phase because they explicitly separate algorithmic invariants from implementation detail.

- Cormen, T. H., Leiserson, C. E., Rivest, R. L. and Stein, C. (2022). *Introduction to Algorithms* (4th ed). MIT Press. (Reference text, no DOI)
- Dijkstra, E. W. (1959). A note on two problems in connexion with graphs. *Numerische Mathematik, 1*, 269–271. https://doi.org/10.1007/BF01386390
- Floyd, R. W. (1962). Algorithm 97: Shortest path. *Communications of the ACM, 5*(6), 345. https://doi.org/10.1145/367766.368168
- Hoare, C. A. R. (1962). Quicksort. *The Computer Journal, 5*(1), 10–15. https://doi.org/10.1093/comjnl/5.1.10
- Knuth, D. E. (1998). *The Art of Computer Programming, Volume 3: Sorting and Searching* (2nd ed). Addison-Wesley. (Reference text, no DOI)


### Appendix G: Loop invariants and proof templates

The examination setting rewards the ability to state invariants crisply, because invariants allow you to justify correctness without narrating every instruction. Below are compact templates that correspond to the implementations in this repository.

#### Selection sort invariant

Let `i` be the index of the current outer loop iteration.

**Invariant:** before the `i`-th iteration begins, the prefix `A[0..i-1]` consists of the `i` smallest elements of the original array and it is sorted in nondecreasing order. The suffix `A[i..n-1]` is a permutation of the remaining elements.

**Initialisation:** for `i = 0` the prefix is empty, so the claim holds vacuously.

**Maintenance:** the inner loop selects `m = argmin_{j in [i,n-1]} A[j]` then swaps `A[i]` and `A[m]`. After the swap, `A[i]` is the smallest element of the previous suffix, so the new prefix `A[0..i]` contains the `i+1` smallest elements and remains sorted because all earlier elements are smaller than or equal to `A[i]`.

**Termination:** when `i = n-1` the prefix has length `n-1` and the remaining suffix has length one, hence the whole array is sorted.

#### Lomuto partition invariant

During partition, maintain two indices `i` and `j` where `j` scans the array and `i` denotes the boundary between elements `<= pivot` and elements `> pivot`.

**Invariant:** at the start of each iteration of the `j` loop, the subarray `A[low..i]` contains elements `<= pivot`, the subarray `A[i+1..j-1]` contains elements `> pivot` and `A[j..high-1]` is unclassified.

At termination, swapping `A[i+1]` with `A[high]` places the pivot between the two regions.

#### Merge invariant

During merge, indices `p` and `q` point into the left and right temporary arrays.

**Invariant:** after `k` assignments into the output region `A[left..right]`, the prefix `A[left..left+k-1]` contains the `k` smallest elements of the multiset `L ∪ R` and it is sorted.

The invariant is maintained because each step selects the smaller head element of the two sorted lists.

#### BFS layer property

If BFS is executed on an unweighted graph with a queue that enqueues newly discovered vertices, then vertices are dequeued in nondecreasing order of their distance from the start vertex. This property follows because all neighbours of a vertex at distance `d` are enqueued after the vertex is dequeued and they are assigned distance `d+1`. No vertex at distance `d+1` can be dequeued before all vertices at distance `d` have been dequeued.

#### Dijkstra invariant

Let `S` be the set of processed vertices.

**Invariant:** for every vertex `u` in `S`, `dist[u]` equals the length of the shortest path from the source to `u`. For every vertex `v` not in `S`, `dist[v]` is the length of the shortest known path whose internal vertices are all in `S`.

The invariant relies on nonnegative weights: when the algorithm selects the unprocessed vertex with minimal tentative distance, no alternative path through unprocessed vertices can decrease it.

### Appendix H: Further reading

The following sources offer rigorous treatments of the algorithms used in this week and are suitable for revision. They are not required for building the code but they support proof writing and complexity analysis.

- Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2009). *Introduction to algorithms* (3rd ed). MIT Press.
- Knuth, D. E. (1998). *The art of computer programming, Volume 3: Sorting and searching* (2nd ed). Addison-Wesley.
- Dijkstra, E. W. (1959). A note on two problems in connexion with graphs. *Numerische Mathematik, 1*, 269–271.
