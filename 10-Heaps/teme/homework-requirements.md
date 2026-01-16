# Week 10 Homework: Heaps and Priority Queues

## Administrative information

- **Submission deadline:** end of teaching week 11 (local course policy applies)
- **Weighting:** 100 points
- **Implementation language:** C (C11)
- **Build assumptions:** GCC or Clang with `-Wall -Wextra -Wpedantic -std=c11`
- **Permitted libraries:** standard C library only (`stdio.h`, `stdlib.h`, `string.h`, `stdint.h`, `stdbool.h`, `time.h` and similar)

You should treat the specification below as part of the assessment. The principal evaluation criterion is correctness under a clear complexity contract. Where your design deviates from the reference design, you must justify the deviation with a precise argument.

## General requirements

1. **Safety and correctness**
   - No undefined behaviour.
   - No memory leaks, double frees or out-of-bounds access.
   - All allocation failures are handled explicitly.

2. **Complexity discipline**
   - Any heap based operation that is expected to be `O(log n)` must remain `O(log n)` in the number of queued elements.
   - Avoid linear scans of the heap except where the specification explicitly permits them.

3. **Reproducibility**
   - Include a small deterministic test harness for each task.
   - Your tests should be runnable with a single compile command.

4. **Documentation**
   - Provide a brief header comment describing the data structures, the invariants and the complexity of each public function.

---

## Homework 1: Task scheduler using a heap based priority queue (50 points)

### Problem statement

Design and implement a task scheduler that selects the next task to execute according to a priority policy. The scheduler maintains a queue of pending tasks. When asked for the next task, it must return the task that is maximal with respect to the chosen policy.

The intended solution uses a binary heap, but you may choose any design that satisfies the complexity contract.

### Data model

Define a `Task` structure with the following fields:

- `int id;` unique identifier
- `int priority;` integer in `[1, 10]` where 10 is highest
- `time_t arrival_time;` time of insertion into the scheduler
- `unsigned int exec_time_ms;` estimated execution duration
- `char description[65];` NUL-terminated string, maximum 64 visible characters

You may add fields if you justify their purpose.

### Scheduler interface

Provide a scheduler API with the following minimal operations:

- `Scheduler *scheduler_create(size_t initial_capacity, Policy policy);`
- `bool scheduler_add_task(Scheduler *s, const Task *t);`
- `bool scheduler_peek_next(const Scheduler *s, Task *out);`
- `bool scheduler_get_next(Scheduler *s, Task *out);`
- `void scheduler_print_stats(const Scheduler *s);`
- `void scheduler_destroy(Scheduler *s);`

The policy may be represented as an enum, a function pointer or a struct of callbacks. The design must permit switching policy without rewriting heap code.

### Scheduling policies

Implement at least two policies.

1. **Priority-first**
   - Primary key: `priority` (higher first)
   - Tie-breaker: earlier `arrival_time` first
   - Secondary tie-breaker: smaller `id` first

2. **Priority with ageing**
   - Each task has a time-dependent effective priority that increases with waiting time in the queue.
   - A simple linear model is acceptable, for example:

     \[
     \pi_{eff}(t) = \min(10, \pi_{base} + \lfloor \alpha \cdot (t - t_{arrival}) \rfloor)
     \]

     where `alpha` is a tunable ageing rate.

   - Tie-breakers should remain stable and deterministic.

You are free to implement a different ageing function (for example logarithmic ageing) but you must specify it clearly.

### Algorithmic expectations

The natural design is a heap ordered by a comparator derived from the chosen policy.

- `scheduler_add_task` should be `O(log n)`
- `scheduler_get_next` should be `O(log n)`
- `scheduler_peek_next` should be `O(1)`

If your ageing policy depends on the current time then you must address how the heap order is maintained when priorities drift. Two acceptable approaches are:

- **Lazy ageing**: compute `pi_eff` only when comparing two tasks. This keeps asymptotic cost but makes the comparator time-dependent.
- **Periodic reheapification**: rebuild the heap at controlled intervals. This introduces occasional `O(n)` work but can be amortised.

You must state which approach you use and why it is correct.

### Pseudocode (reference design)

The following pseudocode expresses the heap based design at a level independent of C.

```
SCHEDULER_ADD(s, task):
  if s.size == s.capacity:
    grow_storage(s)
  s.heap[s.size] <- copy(task)
  s.size <- s.size + 1
  SIFT_UP(s.heap, s.size-1, s.comparator)

SCHEDULER_GET_NEXT(s):
  if s.size == 0:
    return NONE
  top <- s.heap[0]
  s.heap[0] <- s.heap[s.size-1]
  s.size <- s.size - 1
  if s.size > 0:
    SIFT_DOWN(s.heap, s.size, 0, s.comparator)
  return top
```

A comparator for priority-first is:

```
CMP_PRIORITY_FIRST(a, b):
  if a.priority != b.priority:
    return a.priority - b.priority
  if a.arrival_time != b.arrival_time:
    return sign(b.arrival_time - a.arrival_time)  # earlier first
  return sign(b.id - a.id)  # smaller id first
```

### Statistics

Track at least:

- total tasks processed
- average waiting time
- maximum waiting time
- throughput as tasks processed per second of simulated or wall-clock time

You must define precisely how waiting time is measured. A typical definition is:

\[
wait = t_{start} - t_{arrival}
\]

where `t_start` is the time at which the task is extracted for execution.

### Deliverable

Submit a single C file named `homework1_scheduler.c` that contains:

- the scheduler implementation
- a `main` function that demonstrates correctness on a non-trivial example

---

## Homework 2: K-way merge using a min-heap (50 points)

### Problem statement

Given `k` sorted arrays, merge them into a single sorted array using a min-heap. This problem is central to external sorting, multiway merge join in databases and streaming aggregation.

### Data model

A conventional heap node stores:

- `int value;`
- `int array_index;` which input array the value came from
- `int element_index;` index within that array

### Required function

Implement:

```c
bool merge_k_sorted(const int **arrays,
                    const size_t *sizes,
                    size_t k,
                    int *out,
                    size_t *out_size);
```

The function returns `true` on success and writes the merged result to `out` with length `*out_size`.

### Reference algorithm

Initialise the heap with the first element of each non-empty array. Then repeat: extract the smallest element, append it to the output and insert the successor from the same array if one exists.

Pseudocode:

```
MERGE_K(arrays, sizes, k):
  H <- empty min-heap
  for j in 0..k-1:
    if sizes[j] > 0:
      push(H, (arrays[j][0], j, 0))

  out <- empty list
  while H not empty:
    (v, j, i) <- pop_min(H)
    append(out, v)
    if i+1 < sizes[j]:
      push(H, (arrays[j][i+1], j, i+1))

  return out
```

### Complexity contract

Let `N` be the total number of elements over all arrays.

- Heap size is at most `k`.
- Each of the `N` output elements triggers at most one extract and one insert.

Therefore:

- time complexity: `O(N log k)`
- extra space: `O(k)` beyond the output buffer

### Edge cases to handle explicitly

- `k = 0` and `k = 1`
- one or more empty arrays
- arrays with different lengths
- repeated values

### Deliverable

Submit a single C file named `homework2_kmerge.c` that contains:

- the min-heap implementation specialised to merge nodes
- the merge routine
- a `main` function that demonstrates correctness including at least one edge case

---

## Marking scheme

| Criterion | Points |
|---|---:|
| Functional correctness | 40 |
| Preservation of complexity contract | 25 |
| Robust handling of edge cases | 15 |
| Code quality, structure and documentation | 10 |
| No compiler warnings under the required flags | 10 |

### Penalties

- compiler warnings: up to −10
- memory leaks under Valgrind: up to −20
- crashes on valid input: up to −30
- plagiarism or unauthorised collaboration: automatic failure

## Submission checklist

- Files are named exactly as required.
- Programmes compile with the required flags.
- Your `main` functions demonstrate correctness.
- You have run Valgrind and corrected all reported leaks.
- Your comparator logic is deterministic and documented.
