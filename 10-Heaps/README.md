# Week 10: Heaps and Priority Queues

## 🎯 Learning Objectives

By the end of this week, students will be able to:

1. **Remember** the structural and ordering properties that define binary heaps, distinguishing between max-heaps and min-heaps through their invariant conditions
2. **Understand** how the implicit array representation maps a complete binary tree to contiguous memory, deriving parent-child index relationships mathematically
3. **Apply** the heapify operation to restore heap order after element insertion or removal, implementing both iterative and recursive variants
4. **Analyse** the time complexity of heap operations (O(log n) for insertion/extraction, O(n) for build-heap) and justify why Floyd's algorithm achieves linear construction
5. **Evaluate** when a heap-based priority queue outperforms alternative implementations (sorted arrays, balanced BSTs, unsorted lists) across different workload patterns
6. **Create** a fully-functional priority queue ADT supporting dynamic resizing, generic element types through void pointers and custom comparator functions

---

## 📜 Historical Context

The heap data structure emerged from the seminal work on sorting algorithms during the early 1960s, a period of intense research into fundamental computational problems. **J.W.J. Williams** introduced the binary heap in 1964 as the foundational mechanism for **heapsort**, publishing his findings in the journal *Communications of the ACM*. Williams recognised that maintaining a partially ordered complete binary tree enabled efficient in-place sorting with guaranteed O(n log n) worst-case performance—a significant improvement over quicksort's pathological quadratic behaviour.

The following year, **Robert W. Floyd** refined Williams's approach by devising a more elegant heap construction algorithm. Whereas Williams's original method inserted elements one-by-one (yielding O(n log n) construction), Floyd's bottom-up strategy exploits the observation that leaf nodes are trivially valid heaps, propagating the heap property upward in linear time. This insight exemplifies the profound algorithmic improvements achievable through careful analysis of problem structure.

Beyond sorting, heaps found their most enduring application in **priority queue implementations**. The abstract data type—where elements exit according to priority rather than arrival order—appears ubiquitously in operating system schedulers (process management), network routers (packet prioritisation), discrete event simulation engines and graph algorithms such as Dijkstra's shortest path. The heap's elegant balance between structural simplicity and operational efficiency has secured its place in every programmer's fundamental toolkit.

### Key Figure: J.W.J. Williams (1930–2012)

John William Joseph Williams developed heapsort whilst working at the Atomic Weapons Research Establishment in Aldermaston, United Kingdom. A mathematician by training, Williams brought rigorous analytical methods to the nascent field of computer science. His 1964 paper, "Algorithm 232: Heapsort", established both the data structure and its canonical sorting application.

> "The heap is to priority queues what the hash table is to dictionaries—an elegantly simple structure whose utility far exceeds its apparent complexity."

---

## 📚 Theoretical Foundations

### 1. Heap Structure and Properties

A **binary heap** is a complete binary tree satisfying the **heap property**. In a **max-heap**, every parent node stores a value greater than or equal to its children; a **min-heap** inverts this relationship.

```
                    MAX-HEAP                         MIN-HEAP
                       90                               10
                      /  \                             /  \
                    85    70                         20    15
                   /  \  /  \                       /  \  /  \
                  50  60 65  40                    35  25 30  40

        Parent ≥ Children                   Parent ≤ Children
```

The **complete binary tree** property mandates that all levels except possibly the last are fully populated, with nodes in the final level packed leftward. This regularity permits an **implicit array representation**:

```
Index:      0    1    2    3    4    5    6    7    8    9
Array:   [ 90 | 85 | 70 | 50 | 60 | 65 | 40 | 30 | 20 | 55 ]
            │    │    │
            │    │    └── Right child of root
            │    └─────── Left child of root
            └──────────── Root element

Navigation Formulae (0-indexed):
  • Parent of node i:      (i - 1) / 2
  • Left child of node i:  2*i + 1
  • Right child of node i: 2*i + 2
```

### 2. Core Operations

| Operation | Description | Time Complexity |
|-----------|-------------|-----------------|
| `insert` | Add element at end, then "bubble up" | O(log n) |
| `extract_max/min` | Remove root, move last to root, "sift down" | O(log n) |
| `peek` | Return root without removal | O(1) |
| `build_heap` | Convert arbitrary array to heap (Floyd's method) | O(n) |
| `heapsort` | Repeated extraction yields sorted sequence | O(n log n) |

**Sift-Down (Heapify)** pseudocode for max-heap:

```
SIFT_DOWN(A, n, i):
    largest ← i
    left ← 2*i + 1
    right ← 2*i + 2
    
    if left < n AND A[left] > A[largest]:
        largest ← left
    if right < n AND A[right] > A[largest]:
        largest ← right
    
    if largest ≠ i:
        SWAP(A[i], A[largest])
        SIFT_DOWN(A, n, largest)
```

### 3. Floyd's Build-Heap Algorithm

Floyd's insight: leaf nodes (indices ⌊n/2⌋ to n−1) are already valid 1-element heaps. We need only heapify internal nodes from the bottom up:

```
BUILD_MAX_HEAP(A, n):
    for i ← (n/2 - 1) down to 0:
        SIFT_DOWN(A, n, i)
```

**Complexity Analysis:** Although each sift-down costs O(log n), most nodes reside near the bottom where sift distances are small. Summing over all levels yields O(n) total work—a non-obvious result demonstrating that naive "O(n × log n)" intuition can be sharpened through careful analysis.

```
Level:    Nodes at level:    Max sift distance:    Work at level:
  0            1                  log n               log n
  1            2                  log n - 1           2(log n - 1)
  2            4                  log n - 2           4(log n - 2)
  ...
  k           2^k                 log n - k           2^k(log n - k)

Total = Σ 2^k(log n - k) = O(n)  [Geometric series evaluation]
```

---

## 🏭 Industrial Applications

### 1. Operating System Process Scheduling

Modern operating systems employ priority queues to manage process execution. The Linux kernel's Completely Fair Scheduler (CFS) uses a red-black tree, but earlier O(1) schedulers utilised bitmap-indexed arrays of run queues—essentially multiple priority buckets.

```c
/* Simplified priority-based scheduler */
typedef struct {
    int pid;
    int priority;       /* Higher value = higher priority */
    int time_quantum;
} Process;

/* Comparator for max-heap (highest priority first) */
int compare_processes(const void *a, const void *b) {
    const Process *pa = (const Process *)a;
    const Process *pb = (const Process *)b;
    return pa->priority - pb->priority;
}
```

### 2. Dijkstra's Algorithm with Priority Queue

Graph shortest-path algorithms achieve optimal complexity through heap-based priority queues:

```c
/* Priority queue node for Dijkstra */
typedef struct {
    int vertex;
    int distance;
} PQNode;

/* Process vertices in order of increasing distance */
while (!pq_is_empty(pq)) {
    PQNode current = pq_extract_min(pq);
    if (visited[current.vertex]) continue;
    visited[current.vertex] = 1;
    
    /* Relax neighbours */
    for (Edge *e = adj[current.vertex]; e; e = e->next) {
        int new_dist = current.distance + e->weight;
        if (new_dist < dist[e->dest]) {
            dist[e->dest] = new_dist;
            pq_insert(pq, (PQNode){e->dest, new_dist});
        }
    }
}
```

### 3. Event-Driven Simulation

Discrete event simulators schedule future events in timestamp order:

```c
typedef struct {
    double timestamp;
    EventType type;
    void *data;
} SimEvent;

/* Min-heap ordered by timestamp */
int compare_events(const void *a, const void *b) {
    const SimEvent *ea = (const SimEvent *)a;
    const SimEvent *eb = (const SimEvent *)b;
    return (ea->timestamp > eb->timestamp) - (ea->timestamp < eb->timestamp);
}
```

### 4. Memory Allocation (Buddy System)

The buddy allocator tracks free memory blocks using heaps indexed by block size, enabling O(log n) allocation and coalescing.

### 5. Data Stream Processing (Top-K Elements)

Finding the k largest elements in a stream uses a min-heap of size k—new elements displace the smallest when larger:

```c
/* Maintain k largest elements seen so far */
void process_stream_element(MinHeap *heap, int k, int element) {
    if (heap->size < k) {
        heap_insert(heap, element);
    } else if (element > heap_peek(heap)) {
        heap_extract_min(heap);
        heap_insert(heap, element);
    }
}
```

---

## 💻 Laboratory Exercises

### Exercise 1: Generic Priority Queue Implementation

Implement a complete priority queue ADT supporting arbitrary element types through `void *` pointers and function pointer comparators.

**Requirements:**
- Dynamic array storage with automatic resizing (double on overflow, halve when quarter-full)
- Support for both min-heap and max-heap through comparator inversion
- Complete API: `pq_create`, `pq_destroy`, `pq_insert`, `pq_extract`, `pq_peek`, `pq_size`, `pq_is_empty`
- Memory safety: no leaks, no use-after-free, no buffer overflows
- Handle `malloc` failures gracefully

### Exercise 2: Heapsort with Performance Analysis

Implement heapsort and benchmark against other O(n log n) algorithms.

**Requirements:**
- In-place heapsort using Floyd's build-heap
- Comparison counter to verify O(n log n) behaviour empirically
- Benchmark framework measuring execution time across input sizes (100 to 1,000,000 elements)
- Test with sorted, reverse-sorted and random input distributions
- Generate CSV output for plotting

---

## 🔧 Compilation and Execution

```bash
# Build all targets
make

# Build specific targets
make example1          # Complete heap demonstration
make exercise1         # Priority queue exercise
make exercise2         # Heapsort exercise

# Run demonstrations
make run               # Execute example1

# Run automated tests
make test              # Compare outputs against expected results

# Memory leak detection
make valgrind          # Run Valgrind on all executables

# Clean build artefacts
make clean

# Display help
make help
```

---

## 📁 Directory Structure

```
week-10-heaps/
├── README.md                           # This file
├── Makefile                            # Build automation
│
├── slides/
│   ├── presentation-week10.html        # Main lecture (35+ slides)
│   └── presentation-comparativ.html    # Cross-language comparison
│
├── src/
│   ├── example1.c                      # Complete working demonstration
│   ├── exercise1.c                     # Priority queue exercise
│   └── exercise2.c                     # Heapsort exercise
│
├── data/
│   ├── priorities.txt                  # Sample priority data
│   └── numbers.txt                     # Numeric test data
│
├── tests/
│   ├── test1_input.txt                 # Priority queue test input
│   ├── test1_expected.txt              # Expected output
│   ├── test2_input.txt                 # Heapsort test input
│   └── test2_expected.txt              # Expected sorted output
│
├── teme/
│   ├── homework-requirements.md        # Main homework (100 points)
│   └── homework-extended.md            # Bonus challenges (+50 points)
│
└── solution/
    ├── exercise1_sol.c                 # Priority queue solution
    ├── exercise2_sol.c                 # Heapsort solution
    ├── homework1_sol.c                 # Homework 1 solution
    └── homework2_sol.c                 # Homework 2 solution
```

---

## 📖 Recommended Reading

### Essential

- Cormen, T.H., Leiserson, C.E., Rivest, R.L. and Stein, C. (2022) *Introduction to Algorithms*, 4th edn. Cambridge: MIT Press. [Chapter 6: Heapsort]
- Sedgewick, R. and Wayne, K. (2011) *Algorithms*, 4th edn. Boston: Addison-Wesley. [Section 2.4: Priority Queues]

### Advanced

- Williams, J.W.J. (1964) 'Algorithm 232: Heapsort', *Communications of the ACM*, 7(6), pp. 347–348.
- Floyd, R.W. (1964) 'Algorithm 245: Treesort 3', *Communications of the ACM*, 7(12), p. 701.
- Gonnet, G.H. and Munro, J.I. (1986) 'Heaps on Heaps', *SIAM Journal on Computing*, 15(4), pp. 964–971. [Analysis of Floyd's algorithm]

### Online Resources

- Visualgo Heap Visualisation: https://visualgo.net/en/heap
- GeeksforGeeks Heap Tutorial: https://www.geeksforgeeks.org/heap-data-structure/
- OpenDSA Heaps Chapter: https://opendsa-server.cs.vt.edu/OpenDSA/Books/CS3/html/Heaps.html

---

## ✅ Self-Assessment Checklist

Upon completing this week's materials, verify your mastery:

- [ ] I can draw a max-heap and min-heap from an arbitrary array, showing the array-to-tree mapping
- [ ] I can trace the sift-up operation step-by-step when inserting an element
- [ ] I can trace the sift-down operation step-by-step when extracting the root
- [ ] I can implement `build_heap` using Floyd's bottom-up method and explain why it runs in O(n) time
- [ ] I can implement heapsort and prove its O(n log n) worst-case complexity
- [ ] I can design a generic priority queue in C using `void *` and function pointers
- [ ] I can explain when a heap outperforms a balanced BST for priority queue operations
- [ ] I can adapt min-heap code to max-heap code by adjusting the comparator
- [ ] I can implement decrease-key efficiently for Dijkstra's algorithm
- [ ] I can analyse space complexity of both implicit (array) and explicit (pointer-based) heap implementations

---

## 💼 Interview Preparation

### Common Heap Interview Questions

1. **"Implement a heap from scratch."**
   - Demonstrate array-based storage, index calculations, sift-up/down operations
   - Discuss trade-offs: array vs pointer-based implementation

2. **"Find the k-th largest element in an unsorted array."**
   - Approach A: Build max-heap, extract k times. O(n + k log n)
   - Approach B: Build min-heap of size k, process all elements. O(n log k)
   - Discuss which is better for small k vs large k

3. **"Merge k sorted lists efficiently."**
   - Min-heap of k elements (one from each list)
   - Repeatedly extract-min and insert next element from that list
   - Time: O(N log k) where N is total elements

4. **"How would you implement a median-tracking structure?"**
   - Two heaps: max-heap for lower half, min-heap for upper half
   - Balance sizes after each insertion
   - Median accessible in O(1) from heap roots

5. **"What is the time complexity of `heapify` and why?"**
   - Explain the geometric series summation yielding O(n)
   - Contrast with naive O(n log n) upper bound

---

## 🔗 Next Week Preview

**Week 11: Hash Tables** will introduce constant-time average-case dictionary operations through hash functions. We shall examine:
- Hash function design principles (division, multiplication, universal hashing)
- Collision resolution strategies (chaining, open addressing, Robin Hood hashing)
- Load factor management and dynamic resizing
- Applications: symbol tables, caches, set operations

The transition from heaps to hash tables marks a shift from comparison-based to randomised data structures—a fundamental dichotomy in algorithm design.

---

*Prepared for the Algorithms and Programming Techniques course*
*Academy of Economic Studies, Bucharest*
