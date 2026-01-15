# Extended Challenges - Week 10

## 🚀 Advanced Challenges (Optional)

Each correctly solved challenge: **+10 bonus points**

---

## ⭐ Challenge 1: Running Median Tracker (Difficulty: Medium)

### Description

Implement a data structure that efficiently tracks the median of a stream of numbers. Support adding numbers and querying the current median in optimal time.

### Requirements

- `MedianTracker *median_create(void)` - Create tracker
- `void median_add(MedianTracker *mt, int value)` - Add number O(log n)
- `double median_get(MedianTracker *mt)` - Get current median O(1)
- `void median_destroy(MedianTracker *mt)` - Free memory

### Implementation Hint

Use two heaps:
- Max-heap for the lower half of numbers
- Min-heap for the upper half of numbers

Balance the heaps so their sizes differ by at most 1.

### Example

```c
MedianTracker *mt = median_create();
median_add(mt, 5);  // median = 5.0
median_add(mt, 2);  // median = 3.5
median_add(mt, 8);  // median = 5.0
median_add(mt, 1);  // median = 3.5
median_add(mt, 9);  // median = 5.0
printf("Median: %.1f\n", median_get(mt));  // 5.0
median_destroy(mt);
```

### Bonus Points: +10

---

## ⭐ Challenge 2: Heap with Decrease-Key (Difficulty: Medium)

### Description

Extend the priority queue to support efficient `decrease_key` operation, essential for Dijkstra's algorithm. This requires tracking element positions in the heap.

### Requirements

- All standard priority queue operations
- `pq_decrease_key(PriorityQueue *pq, void *element, void *new_key)` - O(log n)
- Handle to element for position tracking
- Works with custom key comparison

### Implementation Hint

Maintain a hash map or array mapping element IDs to their current positions in the heap array. Update positions during sift operations.

### Example

```c
/* For Dijkstra's algorithm */
typedef struct {
    int vertex;
    int distance;
} DijkstraNode;

PriorityQueue *pq = pq_create_with_tracking(...);

DijkstraNode n = {5, 100};
Handle h = pq_insert_tracked(pq, &n);

/* Later, when we find a shorter path */
DijkstraNode updated = {5, 50};
pq_decrease_key(pq, h, &updated);
```

### Bonus Points: +10

---

## ⭐ Challenge 3: D-ary Heap Implementation (Difficulty: Medium)

### Description

Implement a d-ary heap where each node has d children instead of 2. Analyse the trade-offs between different values of d.

### Requirements

- Configurable branching factor d (2, 4, 8, 16)
- All standard heap operations
- Benchmark comparing d=2, d=4, d=8
- Analysis document explaining when higher d is beneficial

### Implementation Hint

Index calculations for d-ary heap (0-indexed):
- Parent of i: `(i - 1) / d`
- k-th child of i: `d * i + k + 1` (for k = 0 to d-1)

### Analysis Questions

1. How does d affect insertion time?
2. How does d affect extraction time?
3. What is the optimal d for different workloads?
4. How does cache performance vary with d?

### Bonus Points: +10

---

## ⭐ Challenge 4: Event-Driven Simulation Engine (Difficulty: Hard)

### Description

Build a discrete event simulation engine using a priority queue for event scheduling. Simulate a simple queueing system (e.g., bank with multiple tellers).

### Requirements

1. **Event Structure**
   - Timestamp (when event occurs)
   - Event type (arrival, service_start, service_end)
   - Associated data (customer ID, teller ID)

2. **Simulation Engine**
   - `sim_create(int num_tellers, double arrival_rate, double service_rate)`
   - `sim_run(Simulation *s, double duration)`
   - `sim_get_stats(Simulation *s, SimStats *stats)`

3. **Statistics**
   - Average wait time
   - Average queue length
   - Teller utilisation
   - Maximum queue length

### Example Output

```
Simulation Results (1000 time units):
─────────────────────────────────────
Customers served:     487
Average wait time:    2.34 units
Max queue length:     12
Teller utilisation:   78.5%
```

### Bonus Points: +10

---

## ⭐ Challenge 5: External Merge Sort (Difficulty: Hard)

### Description

Implement external merge sort for sorting files that don't fit in memory. Use a heap-based K-way merge for the merge phase.

### Requirements

1. **Chunking Phase**
   - Read file in chunks that fit in memory
   - Sort each chunk using heapsort
   - Write sorted chunks to temporary files

2. **Merge Phase**
   - Use min-heap to merge K sorted files
   - Buffered I/O for efficiency
   - Handle arbitrarily large files

3. **Performance**
   - Track I/O operations
   - Measure wall-clock time
   - Test with files larger than available RAM

### Function Signature

```c
int external_sort(const char *input_file, 
                  const char *output_file,
                  size_t memory_limit);
```

### Testing

Create a 1GB file of random integers and sort it with only 64MB of memory available for the sort.

### Bonus Points: +10

---

## 📊 Bonus Point System

| Challenges Completed | Total Bonus |
|---------------------|-------------|
| 1 | +10 points |
| 2 | +20 points |
| 3 | +30 points |
| 4 | +40 points |
| All 5 | +50 points + "Heap Master" badge 🏆 |

---

## 📤 Submission

- Create a separate `.c` file for each challenge
- Name files: `challenge1_median.c`, `challenge2_decrease_key.c`, etc.
- Include test cases demonstrating functionality
- Submit alongside regular homework

---

## 🎯 Evaluation

Each challenge evaluated on:
- Correctness (50%)
- Efficiency - meets specified complexity (30%)
- Code quality (20%)

Partial credit available for incomplete but working solutions.
