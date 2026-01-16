# Week 13: Graph Algorithms — Single-Source Shortest Paths and Negative-Cycle Detection

## Overview

This laboratory package implements two canonical shortest path algorithms in C and uses them to motivate two distinct but tightly related problem classes:

- **Single-source shortest paths in weighted directed graphs with non-negative edge weights** using **Dijkstra’s algorithm** backed by a binary min-heap priority queue.
- **Shortest paths in the presence of negative edge weights** and **negative-cycle detection** using **Bellman–Ford**, with an application to **currency arbitrage detection** through a logarithmic weight transformation.

The repository is designed as a teaching artefact. The assessed exercises emphasise reproducibility of output, explicit invariants and disciplined memory management, rather than hidden library abstractions.

## Repository structure

- `src/`
  - `example1.c` — narrative reference implementation and demonstrations
  - `exercise1.c` — Dijkstra with heap-based priority queue and path reconstruction
  - `exercise2.c` — Bellman–Ford and currency arbitrage cycle reconstruction
- `tests/`
  - `test1_input.txt`, `test1_expected.txt` — regression transcript for Exercise 1
  - `test2_input.txt`, `test2_expected.txt` — regression transcript for Exercise 2
- `data/` — additional inputs for exploratory runs
- `solution/` — instructor reference solutions for exercises and homeworks
- `slides/` — HTML lecture material for Week 13
- `teme/` — homework specifications

## Building and regression testing

### Toolchain assumptions

- C standard: **C11**
- Compiler: **GCC** or a compatible Clang toolchain
- Flags: `-Wall -Wextra -std=c11` with `-lm` where required

### Commands

- Build both exercises:

```bash
make
```

- Run regression tests (golden transcript comparisons):

```bash
make test
```

The test harness is strict. Any deviation in whitespace or line breaks is treated as a behavioural regression.

## Exercise 1: Dijkstra’s algorithm with path reconstruction

### Problem statement

Given a weighted directed graph \(G = (V, E)\) with integer edge weights \(w(e)\ge 0\) and a source vertex \(s\in V\), compute the shortest-path distance \(\delta(s, v)\) from \(s\) to every vertex \(v\) and reconstruct one corresponding shortest path for each reachable vertex.

### Representation

#### Graph: adjacency lists

Each vertex \(u\) owns a linked list of outgoing edges \((u, v, w)\). This representation makes neighbourhood iteration proportional to out-degree:

- Space: \(\Theta(|V| + |E|)\)
- Neighbour scan: \(\Theta(\deg^+(u))\)

The edge node is conceptually:

```text
EdgeNode:
  dest   : int
  weight : int
  next   : EdgeNode*
```

Insertion uses **head-prepending**. This yields deterministic memory behaviour and predictable traversal order given a fixed input edge sequence.

#### Priority queue: binary min-heap with a position map

Dijkstra requires repeated selection of the vertex with minimum tentative distance and repeated key decreases. The implementation uses:

- `nodes[]` — heap array of `(vertex, distance)` pairs
- `position[]` — a vertex-indexed map storing the current heap index of each vertex

The position map is not syntactic decoration. It is the data structure that makes `decrease_key` efficient by avoiding an O(V) search for the vertex’s heap location.

### Algorithmic contract

Dijkstra’s algorithm relies on a greedy property that is correct precisely because all edge weights are non-negative:

- When the algorithm extracts a vertex \(u\) from the min-heap, the current `dist[u]` equals \(\delta(s, u)\).
- No later relaxation can produce a strictly smaller value.

If negative edges exist, this property fails and the algorithm can finalise a vertex prematurely.

### Pseudocode

#### Relaxation

```text
RELAX(u, v, w):
  if dist[u] != INF and dist[u] + w < dist[v]:
    dist[v]   <- dist[u] + w
    parent[v] <- u
```

#### Heap operations

```text
SIFT_UP(i):
  while i > 0:
    p <- floor((i - 1)/2)
    if heap[i].distance >= heap[p].distance:
      break
    swap(heap[i], heap[p])
    update position map for both vertices
    i <- p

SIFT_DOWN(i):
  while true:
    l <- 2*i + 1
    r <- 2*i + 2
    smallest <- i
    if l < size and heap[l].distance < heap[smallest].distance:
      smallest <- l
    if r < size and heap[r].distance < heap[smallest].distance:
      smallest <- r
    if smallest == i:
      break
    swap(heap[i], heap[smallest])
    update position map for both vertices
    i <- smallest
```

#### Dijkstra with heap

```text
DIJKSTRA(G, s):
  for v in V:
    dist[v] <- INF
    parent[v] <- -1

  dist[s] <- 0

  build a min-heap containing all vertices keyed by dist[v]

  while heap not empty:
    (u, du) <- EXTRACT_MIN()
    if du == INF:
      break  // remaining vertices are unreachable

    for each edge (u, v, w) in Adj[u]:
      if v is still in heap and dist[u] + w < dist[v]:
        dist[v] <- dist[u] + w
        parent[v] <- u
        DECREASE_KEY(v, dist[v])

  return dist, parent
```

### Path reconstruction

The `parent[]` array stores a predecessor for each vertex on the chosen shortest-path tree. Reconstructing a path to \(v\) is a reverse walk from \(v\) to \(s\) followed by reversal. The exercise uses a recursive printer, which is equivalent to:

```text
PRINT_PATH(v):
  if v == s:
    output s
    return
  if parent[v] == -1:
    output (no path)
    return
  PRINT_PATH(parent[v])
  output " -> v"
```

### Complexity

Let \(h\) denote heap height. For a binary heap, \(h = \Theta(\log |V|)\).

- Heap build: \(\Theta(|V|)\)
- Each edge can trigger at most one successful decrease-key per relaxation event: \(O(|E| \log |V|)\)
- Extract-min for each vertex: \(O(|V| \log |V|)\)

Total time: \(O((|V| + |E|)\log |V|)\) and space: \(\Theta(|V| + |E|)\).

### Implementation notes and correctness hazards

- `INF` is `INT_MAX`. Before computing `dist[u] + w`, the code checks `dist[u] != INF` to avoid integer overflow.
- The heap `position[]` map must be updated on every swap. If not, `decrease_key` becomes unsound and can corrupt the heap invariant.
- A vertex is considered “in heap” when `position[v] < heap->size`. Extraction marks a vertex as removed by setting its position to the new heap size.

## Exercise 2: Bellman–Ford and currency arbitrage detection

### Arbitrage as a negative cycle

Suppose currencies form a directed graph where an edge \(i \to j\) carries an exchange rate \(r_{ij} > 0\). A cyclic sequence of trades \(v_0 \to v_1 \to \dots \to v_{k-1} \to v_0\) yields a multiplicative gain:

\[
R = \prod_{t=0}^{k-1} r_{v_t v_{t+1}}
\]

Arbitrage exists if \(R > 1\).

To reduce this multiplicative condition to an additive one, define an edge weight:

\[
w_{ij} = -\log(r_{ij})
\]

Then the cycle weight is:

\[
\sum_{t=0}^{k-1} w_{v_t v_{t+1}} = -\log(R)
\]

Therefore \(R > 1\) if and only if \(-\log(R) < 0\), meaning the transformed graph contains a **negative-weight cycle**.

### Bellman–Ford summary

Bellman–Ford performs \(|V| - 1\) global relaxation passes and then checks for a further improvement. If any edge can still be relaxed, at least one negative cycle is reachable from the chosen source.

### Pseudocode

```text
BELLMAN_FORD(G, s):
  for v in V:
    dist[v] <- INF
    parent[v] <- -1
  dist[s] <- 0

  repeat |V| - 1 times:
    updated <- false
    for each edge (u, v, w) in E:
      if dist[u] != INF and dist[u] + w < dist[v]:
        dist[v] <- dist[u] + w
        parent[v] <- u
        updated <- true
    if not updated:
      break

  for each edge (u, v, w) in E:
    if dist[u] != INF and dist[u] + w < dist[v]:
      parent[v] <- u
      return NEGATIVE_CYCLE_FOUND(v)

  return NO_NEGATIVE_CYCLE
```

### Cycle reconstruction

A common subtlety is that the first vertex found to be relaxable may be merely *reachable from* the negative cycle. To ensure entry into the cycle, the code walks backwards through `parent[]` exactly \(|V|\) times, after which the vertex is guaranteed to be on the cycle.

The parent chain encodes predecessor edges. Walking parents traverses the cycle in reverse direction, so the collected vertex sequence is reversed before printing.

The printed cycle is rotated to start at vertex 0 if it appears in the cycle, otherwise it starts at the numerically smallest vertex. This deterministic normalisation supports regression testing while remaining semantically faithful.

### Floating-point considerations

The transformation uses `log` and therefore introduces floating-point error. The implementation uses a small tolerance `EPSILON = 1e-12` in comparisons of the form `dist[u] + w < dist[v] - EPSILON`. This is not a proof device but a pragmatic stabiliser that reduces false positives driven by rounding.

### Profit computation

Given a reconstructed cycle \(C\), profit is computed by multiplying the original rates along the directed edges of the cycle:

\[
\mathrm{profit\%} = (R - 1)\times 100
\]

This is printed to two decimal places.

## Data files

- `data/graph1.txt`, `data/graph2.txt` provide additional directed graphs for manual runs of Exercise 1.
- `data/currencies.txt` provides an exchange matrix suitable for experimenting with Exercise 2.

These files are not used in the regression tests but are appropriate for exploratory validation.

## Engineering checklist

A submission is considered robust when it satisfies all of the following:

- builds with the provided Makefile without warnings
- passes `make test`
- does not leak memory in exercised code paths
- handles invalid input by exiting with a non-zero status and a meaningful message
- maintains algorithmic invariants explicitly (heap order, parent semantics, reachability)

## Notes on determinism

Several algorithmically valid answers can exist for the same problem instance:

- In Dijkstra, multiple shortest paths can tie in total weight.
- In arbitrage detection, multiple negative cycles can coexist.

The regression tests therefore validate one canonical output produced by the deterministic neighbour and edge iteration order in this repository. When extending the code, preserve ordering rules unless you also regenerate the expected transcripts.

## Mathematical preliminaries and notation

Let G = (V, E) be a directed graph with vertex set V = {0, 1, ..., n-1} and edge set E subseteq V x V. A weight function w maps each directed edge (u, v) to a real-valued cost w(u, v). A path p from s to t is a sequence of vertices <v0, v1, ..., vk> with v0 = s and vk = t such that (v_{i-1}, v_i) is in E for all i in {1, ..., k}. The path weight is the sum of its edge weights:

w(p) = sum_{i=1..k} w(v_{i-1}, v_i)

For a fixed source s, the single-source shortest path problem asks for delta(s, v) = min w(p) over all s-to-v paths p, with delta(s, v) = +infty if v is unreachable from s.

Two invariants will appear repeatedly:

- A distance label dist[v] is an upper bound on delta(s, v). It becomes exact when an algorithm can prove optimality.
- A parent pointer parent[v] defines a directed predecessor relation that reconstructs a witness path whose weight equals dist[v] whenever parent pointers are consistent.

The exercises implement these ideas in C in a way that makes invariants explicit and mechanically inspectable.

## Graph representation used in the exercises

Both exercises use an adjacency list representation.

### Data structure

For each vertex u, adj[u] is a singly linked list of outgoing edges (u, v) each carrying an integer weight (Exercise 1) or a transformed floating-point weight (Exercise 2).

In Exercise 1 the edge node is:

```c
typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;
```

The graph owns an array of pointers, one per vertex:

```c
typedef struct {
    int V;
    Edge **adj;
} Graph;
```

### Complexity consequences

- Memory usage is Theta(|V| + |E|) for adjacency lists, which is optimal up to constant factors when the graph is sparse.
- Iterating over outgoing edges from a vertex u is Theta(out_degree(u)). This matters because both Dijkstra and Bellman–Ford are edge-relaxation algorithms.

### Determinism and traversal order

Edges are inserted at the head of each adjacency list. This choice has no effect on the set of shortest path distances but it can change which shortest path is reconstructed when several distinct shortest paths tie in total weight. The regression tests therefore assume the deterministic ordering induced by this insertion policy.

## Exercise 1 in depth: Dijkstra with a binary min-heap

### Algorithmic precondition

Dijkstra’s algorithm requires that all edge weights are non-negative. The implementation does not attempt to repair negative weights because that would change the mathematical problem. Instead, it assumes the precondition and treats violation as undefined behaviour of the algorithm.

A minimal counterexample illustrates the failure mode. Consider edges s->a of weight 1, s->b of weight 2 and b->a of weight -5. Dijkstra would finalise a at distance 1 before discovering the better path s->b->a of weight -3.

### Priority queue design

The priority queue is a binary min-heap over HeapNode records:

```c
typedef struct {
    int vertex;
    int distance;
} HeapNode;
```

The heap stores two arrays:

- nodes[i] is the heap array
- position[v] gives the current heap index of vertex v

This position map makes decrease-key an O(log n) operation without searching.

### Heap invariants

For every heap index i, let left(i) = 2i+1 and right(i) = 2i+2. The heap order invariant is:

nodes[i].distance <= nodes[left(i)].distance and nodes[i].distance <= nodes[right(i)].distance whenever children exist.

The position invariant is:

position[nodes[i].vertex] = i for all valid heap indices i.

Violating the position invariant silently breaks decrease-key. For that reason every swap updates position for both involved vertices.

### Pseudocode for heap operations

Insertion is not used because the heap is built in one pass. The essential operations are extract-min and decrease-key.

```text
EXTRACT_MIN(heap):
  if heap.size == 0:
    return (vertex = -1, distance = INF)

  root = heap.nodes[0]
  last = heap.nodes[heap.size - 1]
  heap.nodes[0] = last
  heap.position[last.vertex] = 0
  heap.size = heap.size - 1
  heap.position[root.vertex] = heap.size
  MIN_HEAPIFY(heap, 0)
  return root
```

```text
DECREASE_KEY(heap, v, new_dist):
  i = heap.position[v]
  heap.nodes[i].distance = new_dist
  while i > 0:
    p = floor((i-1)/2)
    if heap.nodes[p].distance <= heap.nodes[i].distance:
      break
    swap heap.nodes[p] and heap.nodes[i]
    update heap.position for both swapped vertices
    i = p
```

```text
MIN_HEAPIFY(heap, i):
  smallest = i
  l = left(i)
  r = right(i)
  if l < heap.size and heap.nodes[l].distance < heap.nodes[smallest].distance:
    smallest = l
  if r < heap.size and heap.nodes[r].distance < heap.nodes[smallest].distance:
    smallest = r
  if smallest != i:
    swap heap.nodes[i] and heap.nodes[smallest]
    update heap.position for both swapped vertices
    MIN_HEAPIFY(heap, smallest)
```

### Dijkstra pseudocode with explicit invariants

```text
DIJKSTRA(G, s):
  for each v in V:
    dist[v] = INF
    parent[v] = -1
  dist[s] = 0

  heap contains all vertices keyed by dist

  while heap not empty:
    (u, du) = EXTRACT_MIN(heap)
    if du == INF:
      break  // remaining vertices are unreachable

    for each edge (u, v) with weight w:
      if v is in heap and dist[u] + w < dist[v]:
        dist[v] = dist[u] + w
        parent[v] = u
        DECREASE_KEY(heap, v, dist[v])

  return dist and parent
```

A useful loop invariant is: when u is extracted from the heap, dist[u] equals delta(s, u). The proof uses the non-negativity of edge weights and the fact that the heap always selects the minimum tentative distance.

### Path reconstruction semantics

The parent array represents a shortest path tree rooted at s for reachable vertices. For a vertex v != s, parent[v] is the predecessor immediately before v on the reconstructed path from s to v. The function `print_path_recursive` follows parent pointers until it reaches the source.

This representation is compact but it is also delicate: parent pointers are only meaningful when dist values were produced by relaxation. Any change to relaxation conditions must be matched by consistent updates of parent.

### Complexity analysis

Let n = |V| and m = |E|.

- Each extract-min is O(log n) and occurs n times.
- Each edge (u, v) is examined once when u is extracted, giving m relax attempts.
- Each successful relaxation triggers decrease-key, which is O(log n).

Worst-case time complexity is O((n + m) log n). Space complexity is O(n + m).

## Exercise 2 in depth: Bellman–Ford for currency arbitrage

### From multiplicative profit to additive weights

If we exchange currencies along a directed cycle C, the final multiplier is the product of exchange rates along the edges of C. Arbitrage exists precisely when that product exceeds 1.

To detect this with shortest path machinery we transform each rate r into a weight w = -log(r) using the natural logarithm. Then:

- The product of rates along a cycle becomes exp(-sum w).
- A product greater than 1 is equivalent to a negative sum of transformed weights.

Hence arbitrage detection reduces to negative-cycle detection.

### Bellman–Ford algorithmic structure

Bellman–Ford is a relaxation algorithm that does not rely on non-negativity. It can be interpreted as dynamic programming over path length: after k iterations, dist[v] is the shortest path weight from the source to v using at most k edges.

Pseudocode:

```text
BELLMAN_FORD(G, s):
  for v in V:
    dist[v] = INF
    parent[v] = -1
  dist[s] = 0

  repeat n-1 times:
    updated = false
    for each edge (u, v, w):
      if dist[u] != INF and dist[u] + w < dist[v]:
        dist[v] = dist[u] + w
        parent[v] = u
        updated = true
    if not updated:
      break

  for each edge (u, v, w):
    if dist[u] != INF and dist[u] + w < dist[v]:
      negative cycle exists
```

The n-1 bound is sharp: any simple path has at most n-1 edges. A strictly improving relaxation after n-1 iterations implies the presence of a cycle that can be traversed to reduce path cost indefinitely.

### Cycle reconstruction method

When a relaxation indicates a negative cycle, we record a vertex v whose dist improved. That vertex can be outside the cycle but reachable from it via parent pointers. The standard trick is:

- walk parent pointers n times from v to guarantee landing inside the cycle
- then traverse parent pointers until the start vertex repeats

The exercise collects vertices in reverse order because parent pointers point backwards along edges. The collected list is then reversed to obtain the cycle direction.

The printed cycle is additionally rotated to start at vertex 0 when it is part of the cycle, otherwise it is rotated to start at the smallest vertex index. This provides stable output for regression testing.

### Floating-point comparisons

Because weights are floating-point values, strict comparisons are numerically fragile. The implementation uses a small tolerance EPSILON. This does not eliminate all numerical issues but it reduces instability in near-neutral markets where products are extremely close to 1.

### Complexity analysis

For n vertices and m edges:

- Bellman–Ford relaxation phase is O(nm)
- cycle detection scan is O(m)
- reconstruction is O(n)

Space complexity is O(n + m).

## Cross-language correspondence

The algorithms are language-agnostic. The following fragments show how the same abstractions appear in other ecosystems.

### Dijkstra priority queue

Python (heapq uses a binary heap, decrease-key is simulated by pushing duplicates):

```python
import heapq

pq = [(0, source)]
dist = {source: 0}
parent = {source: None}

while pq:
    du, u = heapq.heappop(pq)
    if du != dist.get(u):
        continue
    for v, w in adj[u]:
        nv = du + w
        if nv < dist.get(v, float('inf')):
            dist[v] = nv
            parent[v] = u
            heapq.heappush(pq, (nv, v))
```

C++ (std::priority_queue is a max-heap so values are negated or a custom comparator is used):

```cpp
using Item = std::pair<int, int>; // (dist, vertex)
std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
```

Java (PriorityQueue is a min-heap, decrease-key is usually handled by reinsertion):

```java
PriorityQueue<Node> pq = new PriorityQueue<>(Comparator.comparingInt(n -> n.dist));
```

The laboratory implementation is deliberately explicit about decrease-key through position tracking because that exposes the data structure invariant rather than relying on a library trick.

## Reproducibility and extension

If you extend the laboratory, do so in a way that preserves the following:

- a clear statement of algorithmic preconditions
- explicit invariants for any auxiliary data structure you introduce
- deterministic output format or regenerated expected transcripts

Suggested extensions are listed in `teme/homework-extended.md`.

### Shortest path distance

For a fixed source s, the shortest path distance delta(s, v) is defined as the minimum weight among all directed paths from s to v. If v is unreachable from s then delta(s, v) is defined as infinity. The programmes in this repository represent infinity using large sentinels:

- In Exercise 1 (integer weights): INT_MAX is used as INF.
- In Exercise 2 (floating-point weights after logarithmic transformation): DBL_MAX is used as INF.

The distinction matters because addition must not overflow the sentinel. In Exercise 1 all relaxations guard against INF before performing dist[u] + w.

## Data structures in the exercises

### Adjacency list graph (Exercise 1)

The directed weighted graph is represented by an array of adjacency lists. For each vertex u, g->adj[u] points to the head of a singly linked list whose nodes represent outgoing edges.

C representation:

```c
typedef struct Edge {
    int dest;
    int weight;
    struct Edge *next;
} Edge;

typedef struct {
    int V;
    Edge **adj;
} Graph;
```

Algorithmic consequences:

- Space: Theta(V + E) pointers and edge payloads.
- Neighbour iteration: proportional to out-degree of the current vertex.
- Edge insertion: O(1) by head insertion.
- Edge deletion: O(out-degree) without auxiliary indexing.

Engineering notes:

- The implementation uses calloc for the adjacency array to guarantee initial NULL pointers.
- Each call to add_edge allocates exactly one Edge node and links it into the appropriate list.
- free_graph performs a full traversal of each list and frees every node, then frees the adjacency array and the graph struct.

### Binary min-heap with position map (Exercise 1)

Dijkstra’s algorithm requires a priority queue that supports extracting the currently smallest distance estimate and decreasing a key when a relaxation improves a vertex distance.

A binary heap stored in an array supports both operations in O(log V) time. The complication is that decrease-key is only O(log V) if we can locate the heap cell that stores a given vertex. This is why the implementation maintains an auxiliary array position[] such that:

- position[v] gives the current array index of vertex v inside the heap
- if v is no longer in the heap then position[v] is set to a value that is not less than heap->size

C representation:

```c
typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode *nodes;
    int *position;
    int size;
    int capacity;
} MinHeap;
```

Heap invariants:

1. Structural invariant (complete binary tree): nodes are packed densely in level order.
2. Order invariant (min-heap): for every valid index i,
   - nodes[parent(i)].distance <= nodes[i].distance
3. Position coherence: position[nodes[i].vertex] == i for all i in [0, size).

Index relations:

- parent(i) = (i - 1) / 2
- left(i) = 2*i + 1
- right(i) = 2*i + 2

#### Heapify-down (sift-down)

Heapify-down restores the min-heap order invariant after the root has been overwritten by the last element during extract-min.

Pseudocode:

```text
MIN_HEAPIFY(heap, i):
    smallest = i
    l = 2*i + 1
    r = 2*i + 2

    if l < heap.size and heap.nodes[l].distance < heap.nodes[smallest].distance:
        smallest = l
    if r < heap.size and heap.nodes[r].distance < heap.nodes[smallest].distance:
        smallest = r

    if smallest != i:
        swap heap.nodes[i] and heap.nodes[smallest]
        update heap.position for both vertices
        MIN_HEAPIFY(heap, smallest)
```

The implementation is recursive for clarity. An iterative version is equally valid and avoids recursion depth concerns, although heap depth is only floor(log2 V).

#### Decrease-key (bubble-up)

Decrease-key assigns a smaller distance to a vertex then repairs the heap by bubbling the updated node towards the root.

Pseudocode:

```text
DECREASE_KEY(heap, v, new_dist):
    i = heap.position[v]
    heap.nodes[i].distance = new_dist

    while i > 0:
        p = (i - 1) / 2
        if heap.nodes[p].distance <= heap.nodes[i].distance:
            break
        swap heap.nodes[p] and heap.nodes[i]
        update heap.position for both vertices
        i = p
```

#### Extract-min

Extract-min removes the smallest-distance node at the heap root.

Pseudocode:

```text
EXTRACT_MIN(heap):
    if heap.size == 0:
        return (vertex = -1, distance = INF)

    root = heap.nodes[0]
    last = heap.nodes[heap.size - 1]
    heap.nodes[0] = last
    heap.position[last.vertex] = 0

    heap.size = heap.size - 1
    heap.position[root.vertex] = heap.size

    MIN_HEAPIFY(heap, 0)
    return root
```

The position update for the extracted vertex is the mechanism that makes is_in_heap correct. After the decrement, the extracted vertex has position equal to heap.size which is not less than heap.size and therefore is treated as absent.

## Exercise 1: Dijkstra’s algorithm with path reconstruction

### High-level algorithm

Dijkstra is a greedy algorithm that maintains a set S of vertices whose shortest distance from the source has been finalised. A distance estimate dist[v] is maintained for every vertex v. The algorithm repeatedly selects the vertex u with smallest dist[u] among vertices not yet finalised and relaxes all outgoing edges of u.

The min-heap implements the selection rule efficiently.

### Pseudocode

```text
DIJKSTRA(G, s):
    for each vertex v in G:
        dist[v] = INF
        parent[v] = -1

    dist[s] = 0
    heap = BUILD_MIN_HEAP_FROM(dist)

    while heap not empty:
        (u, du) = EXTRACT_MIN(heap)
        if du == INF:
            break  // remaining vertices are unreachable

        for each edge (u, v) with weight w:
            if v is in heap and dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
                parent[v] = u
                DECREASE_KEY(heap, v, dist[v])

    return dist, parent
```

### Path reconstruction

For each reachable vertex v, parent pointers define a directed arborescence rooted at s:

- parent[s] = -1
- for v != s that is reachable: parent[v] is the predecessor on one shortest path

The programme prints the path by recursion:

```text
PRINT_PATH(parent, v, s):
    if v == s:
        print s
        return
    if parent[v] == -1:
        print (no path)
        return
    PRINT_PATH(parent, parent[v], s)
    print " -> " then v
```

This is a linear-time operation in the number of vertices on the path.

### Correctness sketch

The central loop invariant is that when a vertex u is extracted from the heap, dist[u] equals delta(s, u), the true shortest path distance.

A compact argument proceeds by contradiction:

- Suppose u is extracted with dist[u] > delta(s, u).
- Consider a shortest path from s to u and let x be the first vertex on that path that is not yet extracted when u is extracted.
- Let y be the predecessor of x on the path. By construction, y was extracted earlier.
- When y was extracted, relaxation would have set dist[x] <= delta(s, x).
- Because all edge weights are non-negative, delta(s, x) <= delta(s, u) < dist[u].
- Hence dist[x] < dist[u] which contradicts that u had minimum dist among heap elements.

This argument fails in the presence of negative edges because non-negativity is used to compare delta(s, x) and delta(s, u).

### Complexity

Let V = |V| and E = |E|.

- Building the heap: O(V)
- Each extract-min: O(log V) and occurs V times
- Each decrease-key: O(log V) and occurs at most once per successful relaxation
- Each edge is considered once during neighbour scans

Total time: O((V + E) log V)
Total space: O(V + E)

### Why negative edges break Dijkstra

A two-edge counterexample suffices.

- s -> u has weight 1
- s -> v has weight 2
- v -> u has weight -5

Dijkstra extracts u first with distance 1 then finalises it. The path s -> v -> u has weight -3 which is strictly smaller, but Dijkstra will not revisit finalised vertices. Bellman–Ford does not have this failure mode because it does not rely on finalisation by greedy selection.

## Exercise 2: Bellman–Ford for arbitrage detection

### Algorithmic idea

Bellman–Ford computes shortest paths in graphs that may contain negative edge weights. It is based on a dynamic programming identity: after k iterations, dist[v] equals the shortest path weight among all paths from s to v that use at most k edges.

After V-1 iterations, all simple paths have been accounted for because a simple path contains at most V-1 edges.

### Pseudocode

```text
BELLMAN_FORD(G, s):
    for v in V:
        dist[v] = INF
        parent[v] = -1
    dist[s] = 0

    repeat V-1 times:
        updated = false
        for each edge (u, v) with weight w:
            if dist[u] != INF and dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
                parent[v] = u
                updated = true
        if not updated:
            break

    for each edge (u, v) with weight w:
        if dist[u] != INF and dist[u] + w < dist[v]:
            report negative cycle
```

Time: O(VE) in the worst case.

### Currency arbitrage transformation

Exchange rates along a cycle multiply. A cycle is profitable when the product of rates is greater than 1.

Given a directed exchange rate r(u, v) from currency u to currency v, define the transformed weight:

- w(u, v) = -log(r(u, v))

Then for a cycle C:

- sum over edges in C of w(u, v) = -log(product of rates in C)

Hence:

- product of rates in C > 1  if and only if  sum of transformed weights in C < 0

A profitable arbitrage cycle therefore corresponds exactly to a negative cycle in the transformed graph.

### Cycle reconstruction

After detecting a relaxable edge in the final check, we obtain a vertex v whose distance can still be improved. This vertex is on or reachable from a negative cycle. To extract an actual cycle we use parent pointers.

Procedure:

1. Walk parent pointers V times from v to ensure the resulting vertex lies on the cycle.
2. Starting from that vertex, continue following parent pointers until returning to the start.
3. The collected sequence follows parent pointers and therefore lists the cycle in reverse direction.
4. Reverse the list to obtain the directed trading order.

The programme then rotates the cycle for stable presentation. If vertex 0 is present in the cycle it prints starting at vertex 0, otherwise it prints starting at the numerically smallest vertex index.

### Floating-point considerations

The programme uses an epsilon margin when comparing double values. This does not guarantee correct behaviour under adversarial floating-point scenarios, but it stabilises behaviour for classroom-scale inputs. For high-stakes applications one should use well-designed tolerances or rational log approximations and should include bid, ask and fee modelling.

## Cross-language correspondence

The algorithms are language-independent. The implementations differ mainly in memory management discipline and in the visibility of data structure invariants.

### Dijkstra in Python (heapq)

Python’s standard library provides a heap but not a decrease-key operation. A common technique is to push multiple entries and ignore stale entries.

```python
import heapq

def dijkstra(adj, source):
    INF = 10**18
    n = len(adj)
    dist = [INF] * n
    parent = [-1] * n
    dist[source] = 0

    pq = [(0, source)]
    while pq:
        du, u = heapq.heappop(pq)
        if du != dist[u]:
            continue
        for v, w in adj[u]:
            if du + w < dist[v]:
                dist[v] = du + w
                parent[v] = u
                heapq.heappush(pq, (dist[v], v))
    return dist, parent
```

### Dijkstra in C++ (priority_queue)

C++ has a priority_queue but it is a max-heap by default. One can invert the comparison or store negative distances.

```cpp
#include <queue>
#include <vector>
#include <utility>

using P = std::pair<long long,int>; // (dist, vertex)

std::vector<long long> dijkstra(const std::vector<std::vector<std::pair<int,int>>>& adj, int s) {
    const long long INF = (1LL<<60);
    std::vector<long long> dist(adj.size(), INF);
    dist[s] = 0;

    std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [du, u] = pq.top();
        pq.pop();
        if (du != dist[u]) continue;
        for (auto [v, w] : adj[u]) {
            if (du + w < dist[v]) {
                dist[v] = du + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
```

### Bellman–Ford in Java

```java
import java.util.*;

class Edge {
    int u, v;
    double w;
    Edge(int u, int v, double w) { this.u = u; this.v = v; this.w = w; }
}

static double[] bellmanFord(List<Edge> edges, int n, int s) {
    double INF = 1e300;
    double[] dist = new double[n];
    Arrays.fill(dist, INF);
    dist[s] = 0.0;

    for (int i = 0; i < n - 1; i++) {
        boolean updated = false;
        for (Edge e : edges) {
            if (dist[e.u] < INF && dist[e.u] + e.w < dist[e.v]) {
                dist[e.v] = dist[e.u] + e.w;
                updated = true;
            }
        }
        if (!updated) break;
    }
    return dist;
}
```

## Guidance for extending the laboratory

Several natural extensions allow deeper algorithmic engagement:

- Add support for undirected graphs by inserting symmetric edges.
- Extend Exercise 1 to early terminate when a specific destination is finalised.
- Replace the binary heap with a pairing heap or Fibonacci heap and compare practical costs.
- Extend Exercise 2 to report multiple cycles or the most profitable detected cycle under a specified model.

When implementing extensions, preserve the regression harness or regenerate expected transcripts so that behaviour remains reproducible.
