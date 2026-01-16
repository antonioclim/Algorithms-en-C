# Week 10: Heaps and Priority Queues

## Scope

This week introduces the binary heap as an implicit complete binary tree stored in a contiguous array and develops it into a priority queue abstract data type and into heapsort. The emphasis is on algorithmic invariants, explicit cost models and implementation discipline in C11.

The repository contains three C programmes:

- `src/example1.c`: a complete didactic implementation of an integer max-heap and associated demonstrations
- `src/exercise1.c`: a generic priority queue implemented as a binary heap over dynamically allocated element copies
- `src/exercise2.c`: an in-place heapsort implementation using Floyd’s bottom up heap construction with a comparison counter for empirical analysis

A deterministic test harness compares program output against golden transcripts in `tests/` using `diff`. The tests therefore enforce both semantic correctness and output format stability.

## Learning outcomes

After completing this week you should be able to:

1. Define the structural invariant of a complete binary tree and derive the corresponding index arithmetic for a 0-indexed array representation.
2. State and preserve the heap order invariant for max-heaps and min-heaps.
3. Implement and reason about the `sift_up` and `sift_down` primitives as local repair operations for the heap order property.
4. Explain why bottom up heap construction (Floyd’s method) runs in linear time even though a single `sift_down` can be logarithmic.
5. Implement a priority queue interface over a heap with asymptotically optimal insertion and extraction costs for comparison-based priority queues.
6. Implement heapsort and empirically validate its `O(n log n)` comparison behaviour across several input distributions.

## Mathematical specification

### Data model

A binary heap over a totally ordered set `(K, \preceq)` is a finite sequence `A[0..n-1]` together with an implicit complete binary tree structure induced by index arithmetic.

The structure is complete in the sense that nodes are filled level by level and within the last level from left to right. This property is not merely aesthetic. It is the reason that the representation requires no explicit pointers and supports cache-friendly sequential storage.

### Index calculus for 0-indexed arrays

For any valid node index `i`:

- `parent(i) = floor((i − 1) / 2)` for `i > 0`
- `left(i)   = 2i + 1`
- `right(i)  = 2i + 2`

These relations are derived by observing that level-order enumeration of a complete binary tree assigns consecutive indices to nodes and that each node contributes exactly two child positions in the next level.

### Heap order property

For a **max-heap** the heap order invariant is:

\[
\forall i \in \{0, \ldots, n-1\}:\; (left(i) < n \Rightarrow A[i] \succeq A[left(i)]) \;\wedge\; (right(i) < n \Rightarrow A[i] \succeq A[right(i)])
\]

For a **min-heap** the inequalities are reversed.

The crucial observation is that the heap order property is strictly weaker than a total order over the array. Siblings are not ordered relative to each other in general. As a consequence the heap supports efficient access to the extremal element but not efficient arbitrary search.

## Core algorithms

### Sift-up

`sift_up` restores heap order after inserting an element at the end of the array. Only the path from the insertion index to the root can violate the heap order invariant.

Pseudocode for a max-heap is:

```
SIFT_UP(A, i):
  while i > 0:
    p <- parent(i)
    if A[p] >= A[i]:
      break
    swap A[p] and A[i]
    i <- p
```

The operation performs at most one swap per level and therefore runs in `O(log n)` time.

### Sift-down

`sift_down` restores heap order after replacing the root by a smaller element (max-heap case) and is also the key primitive used by Floyd’s build-heap.

A canonical iterative version is:

```
SIFT_DOWN(A, n, i):
  loop:
    largest <- i
    l <- left(i)
    r <- right(i)

    if l < n and A[l] > A[largest]:
      largest <- l
    if r < n and A[r] > A[largest]:
      largest <- r

    if largest == i:
      break

    swap A[i] and A[largest]
    i <- largest
```

The loop follows a single root-to-leaf path so the worst case running time is `O(log n)`.

### Insert and extract

In a max-heap based priority queue:

- `insert(x)` appends `x` and then calls `sift_up`
- `extract_max()` returns the root, moves the last element to the root, decreases the heap size and then calls `sift_down`

Both operations are `O(log n)`.

### Floyd’s build-heap

Floyd’s bottom up heap construction converts an arbitrary array into a heap by fixing subheaps from the last internal node down to the root.

```
BUILD_MAX_HEAP(A, n):
  for i <- floor(n/2) - 1 down to 0:
    SIFT_DOWN(A, n, i)
```

#### Why the running time is `O(n)`

A naive bound would multiply `O(log n)` by `n/2` internal nodes. The linear bound follows from a sharper accounting argument. Most nodes are near the leaves and have small height so their `sift_down` traversals are short.

Let the heap height be `H = floor(log2 n)`. At depth `d` there are at most `2^d` nodes and each such node has height at most `H - d`. The total work can be bounded by:

\[
\sum_{d=0}^{H} 2^d (H - d) = O(n)
\]

This is an instance of a weighted geometric series where the linear term dominates because the weight decays faster than the node count grows.

### Heapsort

Heapsort sorts an array in ascending order by:

1. building a max-heap
2. repeatedly swapping the maximum element at the root with the last element of the current heap
3. shrinking the heap boundary and restoring heap order at the root

```
HEAPSORT(A, n):
  BUILD_MAX_HEAP(A, n)
  for end <- n-1 down to 1:
    swap A[0] and A[end]
    SIFT_DOWN(A, end, 0)
```

Heapsort runs in `O(n log n)` time in the comparison model and sorts in place. It is not stable.

## Implementation notes for this repository

### `src/example1.c`

`example1.c` implements an integer max-heap with dynamic resizing and demonstrates:

- insertion and extraction
- heap construction
- a standalone heapsort

Its purpose is explanatory. It favours clarity and traceability over minimal code size.

### `src/exercise1.c`: generic priority queue

The priority queue stores elements as independent heap allocated byte-copies. This design has two consequences:

1. the queue owns the lifetime of stored values and releases them in `pq_destroy` and during extraction
2. the API is type-agnostic: it treats values as opaque byte sequences and relies on a comparator callback

#### Comparator contract

A comparator `cmp(a, b)` must return a positive integer when `a` has strictly higher priority than `b` under the intended ordering, a negative integer when `a` has strictly lower priority and zero when they are equivalent. The heap uses this predicate exclusively to maintain heap order. If the comparator is not transitive or not antisymmetric then the heap may not terminate or may behave inconsistently.

#### Memory and resizing model

The internal array is an array of pointers `void **data`. Each insertion allocates `elem_size` bytes, copies the provided object into that memory and stores the pointer.

The array grows geometrically by a factor of 2 when full. The implementation also supports optional shrinking when the occupancy ratio becomes small. Shrinking is deliberately conservative because repeated grow–shrink oscillations can dominate runtime under adversarial workloads.

### `src/exercise2.c`: heapsort with comparison counting

The heapsort implementation includes a global comparison counter. The counter increments exactly when a key-to-key comparison is performed inside heap maintenance. This permits an empirical comparison against the asymptotic model `n log2(n)`.

The file contains two sift-down variants:

- `sift_down`: counts comparisons
- `sift_down_standard`: does not count comparisons and is useful for operations where instrumentation would be misleading

The test transcript expects a specific comparison count for the small demonstration instance. That count is a property of the concrete control flow of the chosen implementation rather than a mathematical invariant. If you change the sift-down strategy you may legitimately change the comparison count even when the sorted output remains correct.

### Deterministic testing

The Makefile target `make test` runs:

- `./exercise1 < tests/test1_input.txt` and diffs against `tests/test1_expected.txt`
- `./exercise2 < tests/test2_input.txt` and diffs against `tests/test2_expected.txt`

Because the tests are transcript-based you must treat output formatting as part of the specification. In particular trailing spaces and blank lines are significant.

## Building, running and testing

```bash
make
make test

# Run the complete worked example
make run

# Optional: compile and run the instructor solutions
make solutions
```

If Valgrind is available on your system you may also run `make valgrind`.

## Typical failure modes and diagnostic checklist

1. **Off-by-one index arithmetic**: errors in `parent`, `left` or `right` silently corrupt heap structure.
2. **Forgetting to update heap size**: insertion and extraction must update the size before calling `sift_up` or `sift_down`.
3. **Swapping values rather than pointers in the generic queue**: swapping the pointed-to bytes is unnecessary and error-prone.
4. **Comparator misuse**: reversing the comparator order converts a max-heap into a min-heap. Mixing both in one queue breaks invariants.
5. **Memory ownership confusion**: if the queue stores pointers to external objects rather than copies then freeing them in `pq_destroy` is incorrect.
6. **Transcript instability**: additional debug prints will cause automated tests to fail.

## Correctness arguments and invariants

The implementation choices in this week are best understood through explicit invariants. Each heap routine is a local transformation that preserves a global property. The proofs below are intentionally concise but they identify the exact statements that should be used when you justify behaviour in a written solution or when you construct unit tests.

### Invariant for `sift_up`

Consider a max-heap stored in `A[0..n-1]` and an index `i` such that every node except possibly along the path from `i` to the root already satisfies the heap order property. This situation arises when a new element has been appended at `A[i]`.

A suitable loop invariant for the iterative version is:

- at the start of each loop iteration the heap order property holds for every index `j` not on the path from the current `i` to the root
- the subtrees rooted at the children of `i` are heaps
- the only potential violation of heap order is between `A[i]` and `A[parent(i)]`

Each swap moves the potential violation strictly upward and cannot introduce a new violation below because the swapped value becomes a parent of a subtree that was already a heap. Termination occurs when either `i = 0` or `A[parent(i)] >= A[i]` which implies the heap order property holds everywhere.

### Invariant for `sift_down`

`sift_down` is invoked when the root of a subheap may be smaller than one of its children. The standard situation is extraction where the last element is moved to the root.

A useful invariant is:

- at the start of each iteration all nodes outside the subtree rooted at the current `i` satisfy heap order
- the left and right subtrees of `i` are heaps
- the only possible heap order violation is at `i` itself

Choosing the larger child and swapping ensures that after the swap the new parent is at least as large as its two children so heap order is restored at the previous location. The violation moves downward into the selected child subtree and the process terminates when `i` becomes a leaf or dominates both children.

### Correctness of Floyd’s build-heap

Build-heap performs `sift_down` on internal nodes in reverse level order. The proof is by induction on the index order.

Let `i` range from `floor(n/2) - 1` down to `0`. At the moment we call `sift_down(A, n, i)` all nodes with indices larger than `i` that are roots of subheaps already satisfy the heap property because:

- every leaf is trivially a heap
- when we process `i` both children of `i` are either leaves or roots of subheaps already heapified

Since `sift_down` assumes the children subtrees are heaps it follows that after the call the subtree rooted at `i` is a heap. When the loop finishes the subtree rooted at `0` is the entire tree and therefore the whole array is a heap.

### Correctness of heapsort

The extraction phase maintains a clear loop invariant.

For a max-heap based heapsort, just before each iteration where the current heap boundary is `end`:

- `A[0..end]` is a valid max-heap
- `A[end+1..n-1]` is sorted in ascending order
- every element in the suffix `A[end+1..n-1]` is greater than or equal to every element in the heap prefix

The swap places the maximum element of the heap at `A[end]`. The subsequent `sift_down(A, end, 0)` restores the heap property over the reduced prefix. By induction the suffix grows by one element per iteration and the final state is a fully sorted array.

## Cost model and empirical instrumentation

Heapsort is often analysed in the comparison model. In `src/exercise2.c` the macro `COMPARE(x, y)` increments a counter and then performs a subtraction. The counter therefore measures key-to-key comparisons that occur inside heap maintenance.

Two caveats are pedagogically important:

1. **Comparison counts are implementation dependent.** Two correct heapsort implementations can legally differ in their number of comparisons because they may select children in different orders or short-circuit in different places.
2. **The counter is not a time measurement.** Branch prediction, cache behaviour and swap costs can dominate runtime even when comparison counts are similar.

For small arrays the comparison count is a useful sanity check because it detects accidental quadratic behaviour. For large arrays you should treat it as one observable among several.

## Comparative perspective: priority queue implementations

A priority queue is specified by the operations `insert`, `peek` and `extract` and optionally `decrease_key` and `merge`. Different concrete data structures realise different points in the asymptotic and constant factor design space.

| Representation | `insert` | `peek` | `extract` | Notes |
|---|---:|---:|---:|---|
| Unsorted array | `O(1)` | `O(n)` | `O(n)` | Good when extractions are rare and inserts are frequent. |
| Sorted array | `O(n)` | `O(1)` | `O(1)` | Good when the queue is mostly read and rarely updated. |
| Balanced BST | `O(log n)` | `O(1)` or `O(log n)` | `O(log n)` | Supports ordered iteration and deletion of arbitrary elements. |
| Binary heap | `O(log n)` | `O(1)` | `O(log n)` | Simple, cache-friendly and usually optimal in practice. |
| Fibonacci heap | amortised `O(1)` | `O(1)` | amortised `O(log n)` | Asymptotically strong for `decrease_key` but heavy constants. |

The binary heap is the canonical choice when you need predictable worst case bounds, compact memory usage and straightforward implementation.

## Cross-language correspondence

This repository is implemented in C because it forces explicit reasoning about representation and ownership. In many production settings a heap is consumed through a standard library interface. The underlying invariants are the same but the surface API differs.

### Python

Python’s `heapq` module implements a min-heap over a list. To obtain max-heap behaviour you typically negate keys or store pairs such as `(-priority, item)`.

```python
import heapq

h = []
heapq.heappush(h, (5, 'a'))
heapq.heappush(h, (1, 'b'))
heapq.heappush(h, (3, 'c'))

# Extract in increasing priority
while h:
    pr, x = heapq.heappop(h)
    print(pr, x)
```

### C++

The C++ Standard Library provides `std::priority_queue` which is a max-heap by default.

```cpp
#include <queue>
#include <vector>

std::priority_queue<int> pq;
for (int x : {5, 3, 8, 1}) pq.push(x);
while (!pq.empty()) {
    std::cout << pq.top() << ' ';
    pq.pop();
}
```

### Java

Java’s `PriorityQueue` is a min-heap by default and can be inverted via a comparator.

```java
import java.util.PriorityQueue;
import java.util.Comparator;

PriorityQueue<Integer> minQ = new PriorityQueue<>();
PriorityQueue<Integer> maxQ = new PriorityQueue<>(Comparator.reverseOrder());
```

The translation exercise is conceptually simple: replace index arithmetic by library calls but keep the mental model of heap order repair operations.


## Reproducibility and instrumentation caveats

The comparison counter in `src/exercise2.c` measures the number of key comparisons performed by the selected control flow. Two correct heapsort implementations can have different counts because they choose different sift-down variants, different tie-breaking rules or different stopping conditions. For this reason the transcript includes a fixed expected count for the specific code in this repository and that value should not be treated as a universal constant for heapsort.

## Further reading

- Williams, J. W. J. (1964). Algorithm 232: Heapsort. *Communications of the ACM, 7*(6), 347–348. https://doi.org/10.1145/512274.3734138
- Floyd, R. W. (1964). Algorithm 245: Treesort 3. *Communications of the ACM, 7*(12), 701. https://doi.org/10.1145/355588.365103
- Fredman, M. L. and Tarjan, R. E. (1987). Fibonacci heaps and their uses in improved network optimization algorithms. *Journal of the ACM, 34*(3), 596–615. https://doi.org/10.1145/28869.28874
- Gonnet, G. H. and Munro, J. I. (1986). Heaps on heaps. *SIAM Journal on Computing, 15*(4), 964–971. https://doi.org/10.1137/0215068
