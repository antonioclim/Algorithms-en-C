# Extended challenges: Week 10

These challenges are optional and are intended for students who want to explore non-trivial extensions of the binary heap beyond the baseline syllabus. Each completed challenge that meets the functional and complexity requirements is worth **+10 bonus points** subject to the course policy on maximum bonus credit.

## General rules

- Each challenge must be submitted as a **separate** C11 source file.
- Each file must compile independently without external headers beyond the standard library.
- Each file must contain a minimal `main` function that exercises the implementation and demonstrates representative edge cases.
- You are assessed on correctness, asymptotic efficiency and engineering quality.

---

## Challenge 1: Running median tracker

### Problem

Design a data structure that maintains the median of a stream of integers under online updates. After each insertion, the current median should be available in `O(1)` time.

### Required API

- `MedianTracker *median_create(void)`
- `void median_add(MedianTracker *mt, int value)`
- `double median_get(const MedianTracker *mt)`
- `void median_destroy(MedianTracker *mt)`

### Core idea

Maintain two heaps:

- a **max-heap** `L` that stores the lower half of the elements
- a **min-heap** `U` that stores the upper half of the elements

Invariant:

- `|size(L) - size(U)| <= 1`
- every element of `L` is `<=` every element of `U`

With this invariant the median is either the root of the larger heap or the average of both roots when the heaps are balanced.

### Pseudocode

```
ADD(mt, x):
  if L is empty or x <= max(L):
    insert L <- x
  else:
    insert U <- x

  # rebalance
  if size(L) > size(U) + 1:
    move max(L) to U
  if size(U) > size(L) + 1:
    move min(U) to L

MEDIAN(mt):
  if size(L) == size(U):
    return (max(L) + min(U)) / 2
  if size(L) > size(U):
    return max(L)
  return min(U)
```

### Complexity target

`median_add`: `O(log n)` amortised time, `median_get`: `O(1)` time.

---

## Challenge 2: Heap with decrease-key

### Problem

Extend the priority queue to support `decrease_key` or more generally `change_key` in `O(log n)` time. This operation is central to Dijkstra’s algorithm and to several shortest path and minimum spanning tree variants.

### Design requirement

A decrease-key operation requires a way to locate an element inside the heap in sublinear time. You therefore need a *handle* that remains valid across swaps.

Two acceptable approaches are:

1. **Explicit handles:** store an integer handle in each node and maintain a `handle -> index` table.
2. **External identifiers:** if tasks have unique ids, maintain `id -> index` in an array or hash table.

### Required API (one possible design)

- `Handle pq_insert_tracked(PriorityQueue *pq, const void *element)`
- `bool pq_decrease_key(PriorityQueue *pq, Handle h, const void *new_key)`

### Correctness constraints

- After any swap in `sift_up` or `sift_down` you must update the position map.
- `pq_decrease_key` must restore heap order using a single `sift_up` from the modified index.

### Pseudocode

```
DECREASE_KEY(pq, h, new_val):
  i <- position[h]
  A[i] <- new_val
  SIFT_UP(pq, i)
```

---

## Challenge 3: D-ary heap

### Problem

Generalise the binary heap to a `d`-ary heap where each node has `d` children. Analyse how `d` changes the trade-off between insertion and extraction.

### Index relations (0-indexed)

- `parent(i) = floor((i - 1) / d)` for `i > 0`
- `child(i, k) = d*i + k + 1` for `k in {0, ..., d-1}`

### Analysis questions

1. How does increasing `d` change the height of the heap?
2. How many comparisons are needed to select the best child during sift-down?
3. For which workloads is larger `d` beneficial given modern cache hierarchies?

---

## Challenge 4: Event-driven simulation engine

### Problem

Implement a discrete event simulation of a queueing system using a priority queue ordered by event time. The simulator should model arrivals, service begins and service completions.

### Required components

- event representation with timestamp and type
- priority queue ordered by timestamp (min-heap)
- simulation state including queue length, server availability and accumulated statistics

### Statistical outputs

- mean waiting time
- mean queue length (time-weighted)
- maximum queue length
- utilisation per server

### Suggested methodology

Use the standard technique of advancing the simulation clock to the next event time and updating time-weighted statistics by integrating over inter-event intervals.

---

## Challenge 5: External merge sort

### Problem

Implement external merge sort for input files that do not fit into memory. Use heapsort for in-memory chunk sorting and a heap-based `K`-way merge for the merge phase.

### Required function signature

```c
int external_sort(const char *input_file, const char *output_file, size_t memory_limit);
```

### Algorithm outline

1. **Run generation:** read the input in chunks of size `<= memory_limit`, sort each chunk and write it as a run to a temporary file.
2. **Multiway merge:** open `K` run files, keep a min-heap of the current head element from each run, repeatedly extract-min and advance that run.

### Complexity target

Total CPU time should be `O(N log N)` comparisons and total I/O should be `O(N/B)` block transfers up to logarithmic factors depending on `K`.

---

## Evaluation rubric

Each challenge is assessed using the same criteria:

- **Correctness:** functional behaviour matches the specification for normal and adversarial inputs.
- **Efficiency:** the stated asymptotic complexity is met and constant factors are reasonable.
- **Engineering quality:** clear invariants, defensive programming and a testable design.

